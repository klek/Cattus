//********************************************************************
//  File:    vulkanCore.cpp
//  Date:    Thu, 07 Mar 2024: 16:59
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#include "vulkanCore.h"
#include "vulkanCommon.h"
#include "vulkanValidation.h"
#include "vulkanSurface.h"
#include "vulkanCommandBuffer.h"
#include "vulkanRenderPass.h"
#include "vulkanShaders.h"
#include "vulkanGraphicsPipeline.h"
#include <set>
#include <map>

namespace muggy::graphics::vulkan::core
{
    namespace // Anonymous namespace, ie only for use in this cpp-file
    {
        using surface_collection = utils::free_list<vulkan_surface>;

        // Constants
        const utils::vector<const char*> deviceExt { 1, VK_KHR_SWAPCHAIN_EXTENSION_NAME };

        // Creating the vulkan command class
        class vulkan_command
        {
        public:
            // Constructors
            vulkan_command() = default;
            MUGGY_DISABLE_COPY_AND_MOVE( vulkan_command );
            explicit vulkan_command( VkDevice device, 
                                     uint32_t queueFamilyIndex,
                                     uint32_t swapchainImageCount )
            {
                VkResult result { VK_SUCCESS };
                //utils::vector<VkCommandBuffer> cmdBuffers;
                m_SwapchainImageCount = swapchainImageCount;

                // Setup the command pool
                VkCommandPoolCreateInfo info { };
                info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
                info.queueFamilyIndex = queueFamilyIndex;
                info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

                result = vkCreateCommandPool( device, &info, nullptr, &m_CmdPool );
                if ( result != VK_SUCCESS )
                {
                    MSG("Failed to create the command pool...");
                    goto _error;
                }
                MSG("Created command pool!");

                // NOTE(klek): Currently only using 1 queue for any 
                //             queue family, so queueIndex is always
                //             set to 0
                vkGetDeviceQueue( core::getLogicalDevice(), 
                                  core::getGraphicsFamilyQueueIndex(), 
                                  0, 
                                  &m_GraphicsQueue );
                vkGetDeviceQueue( core::getLogicalDevice(), 
                                  core::getPresentationFamilyQueueIndex(), 
                                  0, 
                                  &m_PresentationQueue );
                // TODO(klek): Add compute queue in the future

                // Command buffers
                createCmdBuffers( device, queueFamilyIndex );

                // Setup semaphores and fences
                {
                    m_ImageAvailable.resize( m_SwapchainImageCount );
                    m_RenderFinished.resize( m_SwapchainImageCount );
                    m_DrawFences.resize( m_SwapchainImageCount );
                    VkSemaphoreCreateInfo semInfo {};
                    semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

                    // NOTE(klek): Fences here are created in an already 
                    //             signaled state, which indicates to 
                    //             vulkan that the frame has already been
                    //             rendered. This will prevent them from 
                    //             waiting indefinitely to render first 
                    //             frame
                    for ( uint32_t i { 0 }; i < m_SwapchainImageCount; i++ )
                    {
                        // Create semaphore for imageAvailable
                        result = vkCreateSemaphore( device, 
                                                    &semInfo, 
                                                    nullptr, 
                                                    &m_ImageAvailable[ i ] );
                        if ( result != VK_SUCCESS )
                        {
                            goto _error;
                        }
                        // Create semaphore for renderFinished
                        result = vkCreateSemaphore( device, 
                                                    &semInfo, 
                                                    nullptr, 
                                                    &m_RenderFinished[ i ] );
                        if ( result != VK_SUCCESS )
                        {
                            goto _error;
                        }
                        // Create the fence
                        if ( !createFence( device, true, m_DrawFences[ i ] ) )
                        {
                            goto _error;
                        }
                    }

                    // TODO(klek): Investigate if we could do this 
                    //             statically instead of dynamic
                    // Allocate memory for m_SwapchainImageCount
                    m_FencesInFlight = (vulkan_fence**)malloc( sizeof(vulkan_fence) * m_SwapchainImageCount );
                    if ( !m_FencesInFlight )
                    {
                        goto _error;
                    }
                    for ( uint32_t i { 0 }; i < m_SwapchainImageCount; i++ )
                    {
                        m_FencesInFlight[i] = nullptr;
                    }
                }

                return;

            _error:
                release();
            }

            // Member functions
            bool beginFrame( vulkan_surface* surface )
            {
                // Are we currently recreating the swapchain?
                if ( surface->isReCreating() )
                {
                    VkResult result { vkDeviceWaitIdle( core::getLogicalDevice() ) };
                    if ( result != VK_SUCCESS )
                    {
                        MSG("beginFrame() [1] vkDeviceWaitIdle failed...");
                        return false;
                    }
                    MSG("Recreating swapchain");
                    return false;
                }

                // Did the window resize?
                if ( surface->isResized() )
                {
                    VkResult result { vkDeviceWaitIdle( core::getLogicalDevice() ) };
                    if ( result != VK_SUCCESS )
                    {
                        MSG("beginFrame() [2] vkDeviceWaitIdle failed...");
                        return false;
                    }

                    if ( !surface->reCreateSwapchain() )
                    {
                        return false;
                    }
                    
                    MSG("Resized");
                    return false;
                }

                uint32_t frame{ surface->getCurrentFrame() };

                // Wait for fence signaling the current frame is complete
                if ( !waitForFence( core::getLogicalDevice(), 
                                    m_DrawFences[ frame ], 
                                    uint64_invalid_id ) )
                {
                    MSG("Draw fence failure...");
                    return false;
                }

                // Get the next swapchain image
                if ( !surface->nextImageIndex( m_ImageAvailable[ frame ], 
                                               nullptr, 
                                               uint64_invalid_id ) )
                {
                    MSG("Could not get the next swapchain image...");
                    return false;
                }

                // Begin recording commands
                vulkan_cmd_buffer& cmdBuffer{ m_CmdBuffers[ frame ] };
                resetCmdBuffer( cmdBuffer );
                beginCmdBuffer( cmdBuffer, false, false, false );

                // Setup the viewport
                VkViewport viewport{ };
                viewport.x = 0.0f;
                viewport.y = 0.0f;
                viewport.width = (float)surface->getWidth();
                viewport.height = (float)surface->getHeigth();
                viewport.minDepth = 0.0f;
                viewport.maxDepth = 1.0f;
                vkCmdSetViewport( cmdBuffer.cmdBuffer, 0, 1, &viewport );

                // Set up the scissor rectangle
                VkRect2D scissor{ };
                scissor.offset.x = 0;
                scissor.offset.y = 0;
                scissor.extent.width = surface->getWidth();
                scissor.extent.height = surface->getHeigth();
                vkCmdSetScissor( cmdBuffer.cmdBuffer, 0, 1, &scissor );

                // Set up the renderpass
                surface->setRenderpassRenderArea( { 0, 
                                                    0, 
                                                    surface->getWidth(), 
                                                    surface->getHeigth() } );
                surface->setRenderPassClearColor( { 0.0f, 0.0f, 0.0f, 0.0f } );

                renderpass::beginRenderPass( cmdBuffer.cmdBuffer, 
                                             cmdBuffer.cmdState,
                                             surface->renderpass(),
                                             surface->currentFrameBuffer() );

                // Bind the pipeline
                vkCmdBindPipeline( cmdBuffer.cmdBuffer, 
                                   VK_PIPELINE_BIND_POINT_GRAPHICS,
                                   pipeline::getGraphicsPipeline() );

                return true;
            }

            bool endFrame( vulkan_surface* surface )
            {
                uint32_t frame{ surface->getCurrentFrame() };
                vulkan_cmd_buffer& cmdBuffer{ m_CmdBuffers[ frame ] };

                // TODO(klek): Determine if this draw should be a 
                // separate member function
                vkCmdDraw( cmdBuffer.cmdBuffer, 3, 1, 0, 0 );

                renderpass::endRenderPass( cmdBuffer.cmdBuffer, 
                                           cmdBuffer.cmdState, 
                                           surface->renderpass() );
                endCmdBuffer( cmdBuffer );

                // Make sure the previous frame is not using this image
                if ( m_FencesInFlight[ frame ] != VK_NULL_HANDLE )
                {
                    waitForFence( core::getLogicalDevice(), 
                                  *m_FencesInFlight[ frame ], 
                                  uint64_invalid_id );
                }

                // Mark the fence as in use by this frame
                m_FencesInFlight[ frame ] = &m_DrawFences[ frame ];

                // Reset the fence for use in the next frame
                resetFence( core::getLogicalDevice(), m_DrawFences[ frame ] );

                // Setup the VkSubmitInfo structure
                VkSubmitInfo info { };
                info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                info.commandBufferCount = 1;
                info.pCommandBuffers = &cmdBuffer.cmdBuffer;
                info.signalSemaphoreCount = 1;
                info.pSignalSemaphores = &m_RenderFinished[ frame ];
                info.waitSemaphoreCount = 1;
                info.pWaitSemaphores = &m_ImageAvailable[ frame ];
                // Set pipeline flag
                VkPipelineStageFlags flags[1]{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
                info.pWaitDstStageMask = flags;

                // Submit it
                VkResult result { VK_SUCCESS };
                result = vkQueueSubmit( m_GraphicsQueue, 1, &info, m_DrawFences[ frame ].fence );
                if ( result != VK_SUCCESS )
                {
                    MSG("Failed to submit queue...");
                    return false;
                }

                // Set the command buffer to submitted
                updateCmdBufferSubmitted( cmdBuffer );

                // Present the surface
                surface->present( m_ImageAvailable[ frame ], 
                                  m_RenderFinished[ frame ], 
                                  nullptr, 
                                  m_PresentationQueue );

                return true;
            }

            void release( void )
            {
                vkDeviceWaitIdle( core::getLogicalDevice() );

                for ( uint32_t i { 0 }; i < m_SwapchainImageCount ; i++ )
                {
                    vkDestroySemaphore( core::getLogicalDevice(), m_RenderFinished[ i ], nullptr );
                    vkDestroySemaphore( core::getLogicalDevice(), m_ImageAvailable[ i ], nullptr );
                    destroyFence( core::getLogicalDevice(), m_DrawFences[ i ] );
                }
                free( m_FencesInFlight );

                for ( uint32_t i { 0 }; i < m_SwapchainImageCount ; i++ )
                {
                    freeCmdBuffer( core::getLogicalDevice(), m_CmdPool, m_CmdBuffers[ i ] );
                }

                vkDestroyCommandPool( core::getLogicalDevice(), m_CmdPool, nullptr );

            }

            [[nodiscard]] constexpr VkCommandPool const commandPool( ) const 
            {
                return m_CmdPool;
            }

        private:
            void createCmdBuffers( VkDevice device, [[maybe_unused]] uint32_t queueFamilyIndex )
            {
                m_CmdBuffers.resize( m_SwapchainImageCount );

                for ( uint32_t i{ 0 }; i < m_SwapchainImageCount; i++ )
                {
                    if ( m_CmdBuffers[ i ].cmdBuffer )
                    {
                        // TODO(klek): Implement free for command buffer
                        freeCmdBuffer( device, m_CmdPool, m_CmdBuffers[ i ] );
                    }

                    // TODO(klek): Implement allocate for command buffer
                    m_CmdBuffers[ i ] = allocateCmdBuffer( device, m_CmdPool, true );
                }
            }

            bool createFence( VkDevice device, bool signaled, vulkan_fence& fence )
            {
                VkFenceCreateInfo fenceInfo{ };
                fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
                if ( signaled )
                {
                    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
                }
                
                VkResult result = { VK_SUCCESS };

                result = vkCreateFence( device, &fenceInfo, nullptr, &fence.fence );
                if ( result != VK_SUCCESS )
                {
                    MSG("Could not create fence...");
                    return false;
                }

                // Set the signaled 
                fence.signaled = signaled;

                return true;
            }

            void destroyFence( VkDevice device, vulkan_fence& fence )
            {
                vkDestroyFence( device, fence.fence, nullptr );
                fence.fence = nullptr;
                fence.signaled = false;
            }

            bool waitForFence( VkDevice device, vulkan_fence& fence, uint64_t timeout )
            {
                if ( !fence.signaled )
                {
                    VkResult result { vkWaitForFences( device, 1, &fence.fence, true, timeout ) };
                    switch ( result )
                    {
                        case VK_SUCCESS:
                        {
                            fence.signaled = true;
                            return true;
                        }
                        case VK_TIMEOUT:
                        {
                            MSG("Fence timed out...");
                            break;
                        }
                        case VK_ERROR_OUT_OF_HOST_MEMORY:
                        {
                            MSG("Out of host memory error on fence wait...");
                            break;
                        }
                        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                        {
                            MSG("Out of device memory error on fence wait...");
                            break;
                        }
                        case VK_ERROR_DEVICE_LOST:
                        {
                            MSG("Device lost error on fence...");
                            break;
                        }
                        default:
                        {
                            MSG("Unknown error on fence wait...");
                            break;
                        }
                    }
                }
                else 
                {
                    return true;
                }

                return false;
            }

            void resetFence( VkDevice device, vulkan_fence& fence )
            {
                if ( fence.signaled )
                {
                    VkResult result { vkResetFences( device, 1, &fence.fence ) };
                    if ( result != VK_SUCCESS ) 
                    {
                        MSG("Failed to reset fence...");
                        assert( result != VK_SUCCESS );
                    }
                    fence.signaled = false;
                }
            }

            VkCommandPool                       m_CmdPool{ nullptr };
            VkQueue                             m_GraphicsQueue{ nullptr };
            VkQueue                             m_PresentationQueue{ nullptr };
            utils::vector<vulkan_cmd_buffer>    m_CmdBuffers;
            utils::vector<vulkan_fence>         m_DrawFences;
            vulkan_fence**                      m_FencesInFlight;
            utils::vector<VkSemaphore>          m_ImageAvailable;
            utils::vector<VkSemaphore>          m_RenderFinished;
            uint32_t                            m_SwapchainImageCount;
        };

        // Indices (locations) of queue families if they exist
        struct queue_family_indices
        {
            uint32_t                graphicsFamily{ uint32_invalid_id };
            uint32_t                presentationFamily{ uint32_invalid_id };

            bool isValid( void ) 
            {
                return ( graphicsFamily != uint32_invalid_id  &&
                         presentationFamily != uint32_invalid_id );
            }
        };
        
        struct device_group
        {
            VkPhysicalDevice        physicalDevice{ VK_NULL_HANDLE };
            VkDevice                logicalDevice { VK_NULL_HANDLE };
        };

        // ***********************************************************
        // Local variables
        VkInstance                  instance{ nullptr };
        VkDebugUtilsMessengerEXT    debugMessenger{ nullptr };
        device_group                deviceGroup;
        queue_family_indices        queueFamilyIndices;
        vulkan_command              gfxCmd{};
        surface_collection          surfaces{};
        VkFormat                    deviceDepthFormat{ VK_FORMAT_UNDEFINED };

        //************************************************************
        // Local functions

        // Check available extensions
        bool isInstanceExtensionsSupported( utils::vector<const char*> &v )
        {
            // List all the available extensions
            uint32_t extCount { 0 };
            vkEnumerateInstanceExtensionProperties( nullptr, &extCount, nullptr );

            utils::vector<VkExtensionProperties> extensions( extCount );
            vkEnumerateInstanceExtensionProperties( nullptr, &extCount, extensions.data() );

            // Check if all desired extensions exists in extensions
            for ( const auto& check : v )
            {
                bool extFound { false };
                for ( const VkExtensionProperties& ext : extensions )
                {
                    if ( strcmp( check, ext.extensionName ) == 0 )
                    {
                        extFound = true;
                        break;
                    }
                }

                // Did we find the current extension?
                if ( !extFound )
                {
                    return false;
                }
            }

            // Well, we reached the end, so all extensions must have been found
            return true;
        }

        // Sets up the CreateInfo struct for debugging
        void setupDebugCreateInfo( VkDebugUtilsMessengerCreateInfoEXT& info )
        {
            // Return if we are not doing validation layers
            if ( !ENABLE_VALIDATION_LAYERS )
            {
                return;
            }

            info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            info.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
                                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
                                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            info.pfnUserCallback = debugCallback;
            info.pUserData = nullptr;
        }

        void getQueueFamilies( VkPhysicalDevice device, VkSurfaceKHR surface )
        {
            uint32_t queueFamCount { 0 };
            vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamCount, nullptr );

            // List all queue families
            utils::vector<VkQueueFamilyProperties> queueFamList( queueFamCount );
            vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamCount, queueFamList.data() );

            // Check if at least one of each desired queue is found
            uint32_t i { 0 };
            for ( const auto& queueFam : queueFamList )
            {
                // Check if queue family has at least one queue in this family
                // We check for graphics bit here
                if ( ( queueFam.queueCount > 0 ) && 
                     ( queueFam.queueFlags & VK_QUEUE_GRAPHICS_BIT ) )
                {
                    // If queue family is valid, then get index
                    queueFamilyIndices.graphicsFamily = i;
                }

                // Check if queue family supports presentation
                VkBool32 presentationSupport { false };
                vkGetPhysicalDeviceSurfaceSupportKHR( device, i, surface, &presentationSupport );
                // Check if queue is presentation support (can be both 
                // graphics and presentation)
                if ( queueFam.queueCount > 0 && presentationSupport )
                {
                    // If presentation support is valid, then get index
                    queueFamilyIndices.presentationFamily = i;
                }

                // If queue family indices are in a valid state, break loop
                if ( queueFamilyIndices.isValid() )
                {
                    break;
                }

                // Increment index
                i++;
            }
        }

        // Helper function for isDeviceSuitable()
        bool isDeviceExtensionsSupported( VkPhysicalDevice device )
        {
            // Get
            uint32_t extCount { 0 };
            vkEnumerateDeviceExtensionProperties( device, nullptr, &extCount, nullptr );

            if ( extCount == 0 ) 
            {
                return false;
            }

            // Create a list of extension properties based on count
            utils::vector<VkExtensionProperties> extensions( extCount );
            vkEnumerateDeviceExtensionProperties( device, nullptr, &extCount, extensions.data() );

            // Check if given extensions are in the list of available extensions
            for ( const auto& devExt : deviceExt )
            {
                bool hasExt { false };
                for ( const VkExtensionProperties& ext : extensions )
                {
                    if ( strcmp( devExt, ext.extensionName ) == 0 )
                    {
                        hasExt = true;
                        break;
                    }
                }

                // Did we find the current extension?
                if( !hasExt )
                {
                    return false;
                }
            }

            // Well, we reached the end, so all extensions must have been found
            return true;
        }

        // Function to check if a device is suitable compare to our
        // needs
        // TODO(klek): Update this function for our needs
        bool isDeviceSuitable( VkPhysicalDevice device, VkSurfaceKHR surface )
        {
            getQueueFamilies( device, surface );

            bool extSupported { isDeviceExtensionsSupported( device ) };

            bool swapchainValid { false };
            if ( extSupported )
            {
                swapchain_details swapchainDetails = getSwapchainDetails( device, surface );
                swapchainValid = !swapchainDetails.formats.empty() && !swapchainDetails.presentationModes.empty();
            }

            return ( queueFamilyIndices.isValid() && 
                     extSupported                 &&
                     swapchainValid                  );
        }

        uint32_t ratePhysicalDevice( VkPhysicalDevice device, VkSurfaceKHR surface )
        {
            uint32_t score { 0 };
            VkResult result{ VK_SUCCESS };

            // Get physical device properties
            VkPhysicalDeviceProperties deviceProperties{};
            vkGetPhysicalDeviceProperties( device, &deviceProperties );

            // Get physical device features
            VkPhysicalDeviceFeatures deviceFeatures{};
            vkGetPhysicalDeviceFeatures( device, &deviceFeatures );

            // Rating system
            if ( deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU )
            {
                score += 1000;
            }
            
            // Maximum possible size of textures, affect graphics quality
            score += deviceProperties.limits.maxImageDimension2D;

            // Application cannot function without geometry shaders
            if ( !deviceFeatures.geometryShader )
            {
                return 0;
            }

            return score;
        }

        bool getPhysicalDevice( VkSurfaceKHR surface )
        {
            // Get number of physical devices that support vulkan
            uint32_t deviceCount{ 0 };
            VkResult result{ VK_SUCCESS };
            result = vkEnumeratePhysicalDevices( instance, &deviceCount, nullptr );
            if ( result != VK_SUCCESS )
            {
                return false;
            }
            assert( deviceCount > 0 );

            if ( deviceCount == 0 )
            {
                MSG("Failed to find GPUs with vulkan support...");
                return false;
            }

            // Get all the physical devices that support vulkan
            utils::vector<VkPhysicalDevice> devices{ deviceCount };
            result = vkEnumeratePhysicalDevices( instance, &deviceCount, devices.data() );
            if ( result != VK_SUCCESS )
            {
                MSG("Failed to get a list of physical devices...");
                return false;
            }

            // TODO(klek): Modify this loop to instead rate physical 
            //             devices, since there might be more than one
            // std::multimap<uint32_t, VkPhysicalDevice> candidates;
            // for ( const auto& device : devices )
            // {
            //     uint32_t score = ratePhysicalDevice( device, surface );
            //     candidates.insert( std::make_pair( score, device ) );
            // }
            // Best candidate is now in the beginning of the ordered map,
            // thus if the score is zero there is no suitable device at
            // all
            // if ( candidates.rbegin()->first > 0 )
            // {
            //     deviceGroup.physicalDevice = candidates.rbegin()->second;
            // }
            // else 
            // {
            //     MSG("Failed to find a suitable GPU...");
            // }
            // Get the first physical device that meets our requirement
            for ( const auto& device : devices )
            {
                if ( isDeviceSuitable( device, surface ) )
                {
                    // TODO(klek): Add internal variable here
                    deviceGroup.physicalDevice = device;
                    break;
                }
            }
            // Add assert for the device we just added
            assert( deviceGroup.physicalDevice );

            // Check if the device we just added is valid, ie not nullptr
            if ( deviceGroup.physicalDevice == VK_NULL_HANDLE )
            {
                MSG(" Failed to find suitable physical device...");
                return false;
            }

            MSG("Physcial device found successfully!");

            return true;
        }

        bool createLogicalDevice( void )
        {
            // Setup queue creation information
            utils::vector<VkDeviceQueueCreateInfo> infos { };
            std::set<uint32_t> indices 
            { 
                queueFamilyIndices.graphicsFamily, 
                queueFamilyIndices.presentationFamily 
            };

            // Queues the logical device needs to create and info to do so
            for ( uint32_t queueFamilyIndex : indices )
            {
                VkDeviceQueueCreateInfo info { };
                info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                info.queueFamilyIndex = queueFamilyIndex;
                info.queueCount = 1;
                // Vulkan needs to know how to handle multiple queues
                float prio { 1.0f };
                info.pQueuePriorities = &prio;

                infos.push_back( info );
            }

            // Information to crete logical device
            VkDeviceCreateInfo info { };
            info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            info.queueCreateInfoCount = (uint32_t)infos.size();
            info.pQueueCreateInfos = infos.data();
            // NOTE(klek): deviceExt is a constant from the top of this
            //             file and contains the extensions we need
            info.enabledExtensionCount = (uint32_t)deviceExt.size();
            info.ppEnabledExtensionNames = deviceExt.data();

            // Physical device features the logical device will be using
            VkPhysicalDeviceFeatures deviceFeat { };
            // TODO(klek): Specify the device features we need
            info.pEnabledFeatures = &deviceFeat;

            VkResult result { VK_SUCCESS };
            result = vkCreateDevice( deviceGroup.physicalDevice, &info, nullptr, &deviceGroup.logicalDevice );
            if ( result != VK_SUCCESS )
            {
                MSG("Failed to create a logical device...");
                return false;
            }

            MSG("Logical device created successfully!");

            return true;
        }

        bool failedInit( void )
        {
            shutdown();
            return false;
        }
    } // Anonymous namespace, ie only for use in this cpp-file
    
    
    bool initialize( void )
    {
        // Determine which adapter (i.e graphics card) to use
        // Determine what is the maximum feature level that is supported
        // Create a vulkan device (this is a virtual adapter)

        // If we already have an instance, then we shutdown
        if ( instance )
        {
            return failedInit();
        }

        // Initialize volk
        // if ( volkInitialize() != VK_SUCCESS )
        // {
        //     failedInit();
        // }

        //************************************************************
        // Submodules
        // Initialize the shader submodule
        if ( !shaders::initialize() )
        {
            return failedInit();
        }
        // TODO(klek): Add renderpass pipeline
        // TODO(klek): Add graphics pipeline
        //************************************************************

        // Check if validation layers should be used and are 
        // available
        if ( ENABLE_VALIDATION_LAYERS && !isValidationLayerSupported() )
        {
            // TODO(klek): Set this to an error message
            MSG("Validation layers requested, but not available...");
            return failedInit();
        }

        // Set up the application info struct
        VkApplicationInfo appInfo{ };
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        // TODO(klek): Change these values from hardcoded to 
        //             application specific. Perhaps extracted from
        //             users game name
        appInfo.pApplicationName = "Application name";
        appInfo.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
        // TODO(klek): Change these values from hardcoded to 
        //             application specific. Perhaps use defines
        //             in common.h?
        appInfo.pEngineName = "MuggyEngine";
        appInfo.engineVersion = VK_MAKE_VERSION( 1, 0, 0 );
        appInfo.apiVersion = VK_API_VERSION_1_2;

        // Determine the extensions we need to have
        utils::vector<const char*> extensions{ }; 
        MUGGY_VULKAN_SETUP_PLATFORM_EXT( extensions );

        // Add debug layer extensions if enabled
        if ( ENABLE_VALIDATION_LAYERS )
        {
            extensions.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
        }

        // Verify that all the added extensions are supported
        if ( !isInstanceExtensionsSupported( extensions ) )
        {
            MSG("VKInstance does not support the required extensions...");
            return failedInit();
        }

        // Set up the creation info struct
        VkInstanceCreateInfo createInfo{ };
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = static_cast<uint32_t>( extensions.size() );
        createInfo.ppEnabledExtensionNames = extensions.data();

        // Additional debug info struct for use during the instance
        // creation and destruction
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{ };

        // Add the validation layers if they are enabled
        if ( ENABLE_VALIDATION_LAYERS )
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>( validationLayers.size() );
            createInfo.ppEnabledLayerNames = validationLayers.data();

            // Setup the debug callback
            setupDebugCreateInfo( debugCreateInfo );
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }
        else 
        {
            createInfo.enabledLayerCount = 0;
            createInfo.ppEnabledLayerNames = nullptr;
            createInfo.pNext = nullptr;
        }

        // Create the instance
        VkResult result = vkCreateInstance( &createInfo, nullptr, &instance );

        // Check that the instance got created correctly
        if ( result != VK_SUCCESS )
        {
            // TODO(klek): Set this to an error message
            MSG("Failed to create vulkan instance...");
            return failedInit();
        }

        // Volk load instance
        //volkLoadInstance( instance );

        MSG("Vulkan instance has been created!");

        // Add the debug messenger if validation layers are enabled
        if ( ENABLE_VALIDATION_LAYERS )
        {
            VkDebugUtilsMessengerCreateInfoEXT debugInfo{ };
            setupDebugCreateInfo( debugInfo );
            result = createDebugUtilsMessengerExt( instance, 
                                                   &debugInfo, 
                                                   nullptr, 
                                                   &debugMessenger);
            if ( result != VK_SUCCESS )
            {
                MSG("Failed to setup vulkan debug messenger...");
                return failedInit();
            }

            MSG("Vulkan validation layer has been created!");
        }

        MSG("Vulkan has been successfully initialized!");

        return true;
    }

    void shutdown( void )
    {
        // Release the command
        gfxCmd.release();

        // Shutdown submodules
        shaders::shutdown();
        // TODO(klek): Add the graphics pipeline
        //pipeline::shutdown( deviceGroup.logicalDevice );

        // TODO(klek): Add renderpass 

        // TODO(klek): Destroy the device we have created
        vkDestroyDevice( deviceGroup.logicalDevice, nullptr );

        // TODO(klek): Destroy any validation layers we created
        if ( ENABLE_VALIDATION_LAYERS )
        {
            // Destroy the debug messenger
            destroyDebugUtilsMessengerExt( instance, debugMessenger, nullptr );
        }

        // Destroying the instance
        vkDestroyInstance( instance, nullptr );
    }

    bool createDevice( VkSurfaceKHR surface )
    {
        // A device should only be create once
        if ( deviceGroup.logicalDevice || deviceGroup.physicalDevice ) 
        {
            return true;
        }
        assert( !deviceGroup.logicalDevice && !deviceGroup.physicalDevice );

        return ( getPhysicalDevice( surface ) && createLogicalDevice() );
    }

    bool createGraphicsCommand( uint32_t swapchainBufferSize )
    {
        // This should only be created once
        if ( gfxCmd.commandPool() )
        {
            return true;
        }
        assert( !gfxCmd.commandPool() );

        // Create a new class in place of the default constructed one
        new ( &gfxCmd ) vulkan_command( deviceGroup.logicalDevice, 
                                        queueFamilyIndices.graphicsFamily,
                                        swapchainBufferSize );
        // Check that it got created
        if ( !gfxCmd.commandPool() )
        {
            return false;
        }

        return true;
    }

    bool detectDepthFormat( VkPhysicalDevice physicalDevice )
    {
        // Check if there is already a format defined
        if ( deviceDepthFormat != VK_FORMAT_UNDEFINED )
        {
            return true;
        }

        const uint32_t count { 3 };
        VkFormat formats[ count ]
        { 
            VK_FORMAT_D32_SFLOAT, 
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT
        };
        uint32_t flags { VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT };

        for ( uint32_t i { 0 }; i < count; i++ )
        {
            VkFormatProperties prop;
            vkGetPhysicalDeviceFormatProperties( physicalDevice, formats[ i ], &prop );

            if ( ( prop.linearTilingFeatures & flags ) == flags )
            {
                deviceDepthFormat = formats[ i ];
                return true;
            }
            else if ( ( prop.optimalTilingFeatures & flags ) == flags )
            {
                deviceDepthFormat = formats[ i ];
                return true;
            }
        }

        // Could not find a format
        return false;
    }

    int32_t findMemoryIndex( uint32_t type, uint32_t flags )
    {
        VkPhysicalDeviceMemoryProperties prop;
        vkGetPhysicalDeviceMemoryProperties( deviceGroup.physicalDevice, &prop );

        for ( uint32_t i { 0 }; i < prop.memoryTypeCount; i++ )
        {
            if ( ( type & BIT( i ) ) && 
                 ( ( prop.memoryTypes[ i ].propertyFlags & flags ) == flags ) )
            {
                return i;
            }
        }

        MSG("Cannot find memory type...");
        return -1;
    }

    //****************************************************************
    // Helper functions
    uint32_t getGraphicsFamilyQueueIndex( void )
    {
        return queueFamilyIndices.graphicsFamily;
    }

    uint32_t getPresentationFamilyQueueIndex( void )
    {
        return queueFamilyIndices.presentationFamily;
    }

    VkFormat getDepthFormat( void )
    {
        return deviceDepthFormat;
    }

    VkPhysicalDevice getPhysicalDevice( void )
    {
        return deviceGroup.physicalDevice;
    }

    VkDevice getLogicalDevice( void )
    {
        return deviceGroup.logicalDevice;
    }

    VkInstance getInstance( void )
    {
        return instance;
    }

    //****************************************************************
    // Low-level functions for vulkan surfaces
    surface createSurface( platform::window window )
    {
        assert( window.isValid() );
        // Add the bare surface to the collection of surfaces
        surface_id id { surfaces.add( window ) };
        assert( id::isValid( id ) );
        // Use the id we got
        surfaces[ id ].create( instance );

        return surface { id };
    }

    void removeSurface( surface_id id )
    {
        assert( id::isValid( id ) );
        surfaces.remove( id );
    }

    void resizeSurface( surface_id id, uint32_t width, uint32_t height )
    {
        assert( id::isValid( id ) );
        // TODO(klek): Add usage of width and height for resizing
        surfaces[ id ].resize();
    }

    uint32_t getSurfaceWidth( surface_id id )
    {
        assert( id::isValid( id ) );
        return surfaces[ id ].getWidth();
    }

    uint32_t getSurfaceHeigth( surface_id id )
    {
        assert( id::isValid( id ) );
        return surfaces[ id ].getHeigth();
    }

    void renderSurface( surface_id id /*, [[maybe_unused]] frame_info info*/ )
    {
        if ( gfxCmd.beginFrame( &surfaces[ id ] ) )
        {
            // TODO(klek): Add binding of the graphics pipeline
            // vkCmdBindPipeline( gfxCmd.commandBuffer, 
            //                    VK_PIPELINE_BIND_POINT_GRAPHICS, 
            //                    graphicsPipeline );
            // TODO(klek): Add the drawCmd
            // vkCmdDraw( gfxCmd.commandBuffer, 3, 1, 0, 0 );
            // ..

            gfxCmd.endFrame( &surfaces[ id ] );
        }
    }

} // namespace muggy::graphics::vulkan::core
