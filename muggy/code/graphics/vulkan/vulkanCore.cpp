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
//#include "vulkanSurface.h"
#include "vulkanSwapchain.h"
#include "vulkanCommandBuffer.h"
#include "vulkanShaders.h"
#include "vulkanHelpers.h"
#include "vulkanContent.h"

namespace muggy::graphics::vulkan::core
{
    namespace // Anonymous namespace, ie only for use in this cpp-file
    {
        using surface_collection = utils::free_list<vulkan_surface>;
        // Constants
        const utils::vector<const char*> deviceExt { 1, VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        enum vulkan_queue_type : uint8_t
        {
            // Queues
            VULKAN_GRAPHICS_QUEUE   = 0x01,
            VULKAN_COPY_QUEUE       = 0x02,
            VULKAN_COMPUTE_QUEUE    = 0x04,
            VULKAN_QUEUE_MAX        = 0x08,
            // Features (such as present)
            VULKAN_PRESENT_SUPPORT  = 0x10,
            VULKAN_PRESENT_MAX      = 0x80
        };

        //************************************************************
        // Local structs
        // Simple class to store a queueFamily index and if this 
        // specific queue family supports presentation
        class vulkan_queue_family
        {
        public:
            vulkan_queue_family( ) = delete;
            explicit vulkan_queue_family( const VkQueueFlagBits& type )
             : m_QueueType ( type )
            {}
            MUGGY_DISABLE_COPY_AND_MOVE( vulkan_queue_family );

            // Set functions
            constexpr void setQueueType( VkQueueFlagBits type ) { m_QueueType = type; }
            constexpr void setQueueFamilyIndex( uint32_t index ) { m_QueueFamilyIndex = index; }
            constexpr void setPresentationSupport( bool value ) { m_SupportsPresentation = value; }

            // Get functions
            constexpr VkQueueFlagBits getQueueType( void ) const { return m_QueueType; }
            constexpr uint32_t getQueueFamilyIndex( void ) const { return m_QueueFamilyIndex; }
            constexpr bool isPresentationSupported( void ) const { return m_SupportsPresentation; }
            constexpr bool isValid( void ) const { return ( m_QueueFamilyIndex != uint32_invalid_id ); }

        private:

            uint32_t                        m_QueueFamilyIndex { uint32_invalid_id };
            bool                            m_SupportsPresentation { false };
            VkQueueFlagBits                 m_QueueType { };
        };

        // Struct to store indices (locations) of queue families that
        // has been requested, if the exist
        struct queue_family_indices
        {
            uint32_t                        graphicsFamily{ uint32_invalid_id };
            // NOTE(klek): There is no actual present queue...only
            //             queue families that support presentation!
            uint32_t                        presentationFamily{ uint32_invalid_id };
            // TODO(klek): Add the copy and compute to the isValid()-
            //             function below, when they are supported
            uint32_t                        copyFamily{ uint32_invalid_id };
            uint32_t                        computeFamily{ uint32_invalid_id };

            bool isValid( void )
            {
                return ( graphicsFamily != uint32_invalid_id  &&
                         presentationFamily != uint32_invalid_id );
            }
        };

        // Struct to store a physical device handle and the associated
        // logical device handle
        struct device_group
        {
            VkPhysicalDevice                physicalDevice { VK_NULL_HANDLE };
            VkDevice                        logicalDevice { VK_NULL_HANDLE };
        };

        // Small struct that encapsulates information
        // about each "virtual" frame
        struct vulkan_frame
        {
            VkCommandBuffer             m_CmdBuffer { VK_NULL_HANDLE };
            // VkFramebuffer               m_FrameBuffer { VK_NULL_HANDLE };
            VkSemaphore                 m_ImageAvailableSem { VK_NULL_HANDLE };
            VkSemaphore                 m_RenderFinishedSem { VK_NULL_HANDLE };
            VkFence                     m_Fence { VK_NULL_HANDLE };

            ~vulkan_frame()
            {
                assert( !m_CmdBuffer );
                // assert( !m_Framebuffer );
                assert( !m_ImageAvailableSem );
                assert( !m_RenderFinishedSem );
                assert( !m_Fence );
            }

            bool wait( const VkDevice& device )
            {
                VkResult result { vkWaitForFences( device, 1, &m_Fence, true, uint64_invalid_id ) };
                switch ( result )
                {
                    case VK_SUCCESS:                    return true;
                    case VK_TIMEOUT:                    MSG("Fence timed out..."); break;
                    case VK_ERROR_OUT_OF_HOST_MEMORY:   MSG("Out of host memory error on fence wait..."); break;
                    case VK_ERROR_OUT_OF_DEVICE_MEMORY: MSG("Out of device memory error on fence wait..."); break;
                    case VK_ERROR_DEVICE_LOST:          MSG("Device lost error on fence..."); break;
                    default:                            MSG("Unknown error on fence wait..."); break;
                }

                return false;
            }

            bool resetFence( const VkDevice& device )
            {
                VkResult result { vkResetFences( device, 1, &m_Fence ) };
                if ( VK_SUCCESS != result )
                {
                    MSG("Could not reset the fence...");
                    return false;
                }
                return true;
            }

            void release( const VkDevice& device, VkCommandPool cmdPool )
            {
                // If a member has a value, we destroy/free it
                // if ( VK_NULL_HANDLE != m_FrameBuffer )
                // {
                //     vkDestroyFramebuffer( device, m_FrameBuffer, nullptr );
                // }
                if ( VK_NULL_HANDLE != m_CmdBuffer )
                {
                    vkFreeCommandBuffers( device, cmdPool, 1, &m_CmdBuffer );
                    m_CmdBuffer = VK_NULL_HANDLE;
                }
                if ( VK_NULL_HANDLE != m_ImageAvailableSem )
                {
                    vkDestroySemaphore( device, m_ImageAvailableSem, nullptr );
                    m_ImageAvailableSem = VK_NULL_HANDLE;
                }
                if ( VK_NULL_HANDLE != m_RenderFinishedSem )
                {
                    vkDestroySemaphore( device, m_RenderFinishedSem, nullptr );
                    m_RenderFinishedSem = VK_NULL_HANDLE;
                }
                if ( VK_NULL_HANDLE != m_Fence )
                {
                    vkDestroyFence( device, m_Fence, nullptr );
                    m_Fence = VK_NULL_HANDLE;
                }
            }
        };

        //************************************************************
        // Class to encapsulate a commandpool and frame handling
        class vulkan_command
        {
        public:
            vulkan_command() = default;
            MUGGY_DISABLE_COPY_AND_MOVE( vulkan_command );
            explicit vulkan_command( const VkDevice& device,
                                     uint8_t type )
             :
                m_FrameIndex( 0 ),
                m_Device( device )
            {
                // Internal variables needed during this function
                VkResult result { VK_SUCCESS };
                uint32_t queueFamilyIndex { uint32_invalid_id };
                VkCommandPoolCreateInfo info { };

                // Setup the queue
                queueFamilyIndex = getQueueIndex( device, type );
                // Check which queue type this command should belong too
                // Queue indices must match for a given commandpool
                // Check that they do
                if ( uint32_invalid_id == queueFamilyIndex  )
                {
                    MSG("Failed to setup queues..");
                    goto _error;
                }
                // We have a valid queue, check if present was
                // set, and if so does the queue support it?
                if ( VULKAN_PRESENT_SUPPORT & type )
                {
                    m_PresentSupport = isPresentSupported( queueFamilyIndex );
                    if ( !m_PresentSupport )
                    {
                        MSG("Presentation requested, but not supported");
                        goto _error;
                    }
                }

                // Setup commandpool create info structure
                // NOTE(klek): RESET_COMMAND_BUFFER_BIT indicates that
                //             buffers from this pool, might be reset
                //             individually
                //             CREATE_TRANSIENT_BIT indicates that
                //             buffers from this pool are short-lived,
                //             will be often recorded and reset (re-recorded)
                info = vulkan_utils::setCmdPoolCreateInfo( queueFamilyIndex,
                                                           ( VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT |
                                                             VK_COMMAND_POOL_CREATE_TRANSIENT_BIT ) );

                result  = vkCreateCommandPool( device, &info, nullptr, &m_CmdPool );
                if( VK_SUCCESS != result )
                {
                    MSG("Failed to create the command pool...");
                    goto _error;
                }
                MSG("Created a command pool!");

                // Now we need to setup the virtual frames with everything
                // apart from the actual framebuffer which will be allocated
                // during each frames rendering
                for ( uint32_t i { 0 }; i < constants::FRAME_BUFFER_COUNT; i++ )
                {
                    // Get a reference to the first frame
                    vulkan_frame& frame { m_Frames[ i ] };
                    // Allocate the command buffer
                    if ( !createCmdBuffer( device, true, frame.m_CmdBuffer ) )
                    {
                        goto _error;
                    }
                    // Setup semaphores
                    if ( !createSemaphore( device, frame.m_ImageAvailableSem ) )
                    {
                        goto _error;
                    }
                    if ( !createSemaphore( device, frame.m_RenderFinishedSem ) )
                    {
                        goto _error;
                    }
                    // Setup fence
                    if ( !createFence( device, true, frame.m_Fence ) )
                    {
                        goto _error;
                    }
                }

                return;
            _error:
                release( );
            }

            ~vulkan_command()
            {
                assert( !m_CmdPool );
            }

            bool beginFrame( void )
            {
                // Local variables
                VkResult result { VK_SUCCESS };
                // Get a reference to the current frame
                vulkan_frame& frame = m_Frames[ m_FrameIndex ];

                // Wait for the fence
                if ( !frame.wait( m_Device ) )
                {
                    return false;
                }
                // Reset the fence
                if ( !frame.resetFence( m_Device ) )
                {
                    return false;
                }
                // Reset the command buffer
                result = vkResetCommandBuffer( frame.m_CmdBuffer, 0 );
                if ( VK_SUCCESS != result )
                {
                    return false;
                }
                // Begin the command buffer
                VkCommandBufferBeginInfo info {
                    vulkan_utils::setCmdBufferBeginInfo( VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT )
                };
                result = vkBeginCommandBuffer( frame.m_CmdBuffer, &info );
                if ( VK_SUCCESS != result )
                {
                    return false;
                }

                return true;
            }

            bool endFrame( vulkan_surface& surface )
            {
                // Local variables
                VkResult result { VK_SUCCESS };
                // Get a reference to the current frame
                vulkan_frame& frame = m_Frames[ m_FrameIndex ];

                // Here we first need to end the current commandbuffer
                result = vkEndCommandBuffer( frame.m_CmdBuffer );
                if ( VK_SUCCESS != result )
                {
                    return false;
                }
                // Then we need to submit the command buffer
                VkSubmitInfo info {
                    vulkan_utils::setSubmitInfo( frame.m_CmdBuffer,
                                                 frame.m_ImageAvailableSem,
                                                 frame.m_RenderFinishedSem )
                };
                // Set the pipeline flag
                VkPipelineStageFlags flags[ 1 ] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
                info.pWaitDstStageMask = flags;
                // Submit the queue
                result = vkQueueSubmit( m_Queue, 1, &info, frame.m_Fence );
                if ( VK_SUCCESS != result )
                {
                    MSG("Failed to submit queue...");
                    return false;
                }
                // Now we present the surface
                // NOTE(klek): Currently we are not checking if the
                //             presenting is supported for this queue...
                surface.present( m_Queue, frame.m_RenderFinishedSem );

                // Increment the current frame, or roll over if we are
                // at maximum
                m_FrameIndex = ( m_FrameIndex + 1 ) % constants::FRAME_BUFFER_COUNT;
                return true;
            }

            void release( void )
            {
                vkDeviceWaitIdle( core::getLogicalDevice() );

                // Free the members of each vulkan frame
                for ( uint32_t i { 0 }; i < constants::FRAME_BUFFER_COUNT; i++ )
                {
                    m_Frames[ i ].release( core::getLogicalDevice(), m_CmdPool );
                }

                // Release the cmdPool
                if ( VK_NULL_HANDLE != m_CmdPool )
                {
                    vkDestroyCommandPool( core::getLogicalDevice(), m_CmdPool, nullptr );
                    m_CmdPool = VK_NULL_HANDLE;
                }
            }

            // Accessor functions
            [[nodiscard]] constexpr VkCommandPool const getCmdPool( void ) const { return m_CmdPool; }
            [[nodiscard]] constexpr VkCommandBuffer const getCurrentCmdBuffer( void ) const { return m_Frames[ m_FrameIndex ].m_CmdBuffer; }
            [[nodiscard]] constexpr VkFence const getCurrentFence( void ) const { return m_Frames[ m_FrameIndex ].m_Fence; }
            [[nodiscard]] constexpr VkSemaphore const getCurrentImageSemaphore( void ) const { return m_Frames[ m_FrameIndex ].m_ImageAvailableSem; }
            [[nodiscard]] constexpr VkSemaphore const getCurrentRenderSemaphore( void ) const { return m_Frames[ m_FrameIndex ].m_RenderFinishedSem; }
            [[nodiscard]] constexpr uint32_t getCurrentFrame( void ) const { return m_FrameIndex; }

        private:
            uint32_t getQueueIndex( VkDevice device, uint8_t type )
            {
                uint32_t index { uint32_invalid_id };

                for ( uint32_t i { 0 }; !( VULKAN_QUEUE_MAX & BIT(i) ) ; i++ )
                {
                    uint32_t tempIndex { uint32_invalid_id };
                    VkQueue* tempQueue { VK_NULL_HANDLE };
                    // Is bit set in type?
                    switch ( BIT(i) & type )
                    {
                        case VULKAN_GRAPHICS_QUEUE:
                        {
                            tempIndex = core::getGraphicsFamilyQueueIndex();
                            tempQueue = &m_Queue;
                            break;
                        }
                        case VULKAN_COPY_QUEUE:
                        {
                            tempIndex = core::getCopyFamilyQueueIndex();
                            tempQueue = &m_Queue;
                            break;
                        }
                        case VULKAN_COMPUTE_QUEUE:
                        {
                            tempIndex = core::getComputeFamilyQueueIndex();
                            tempQueue = &m_Queue;
                            break;
                        }
                        case VULKAN_QUEUE_MAX:
                        {
                            tempIndex = uint32_invalid_id;
                            tempQueue = VK_NULL_HANDLE;
                            break;
                        }
                        default:
                        {
                            // The current bit is not set, which indicate
                            // we should continue
                            continue;
                        }
                    }
                    // Check if we have already set a queueFamilyIndex
                    if ( uint32_invalid_id != index )
                    {
                        // Check if the new index is same as the already saved
                        if ( index != tempIndex )
                        {
                            // This mean that queues belong to different families
                            // and cannot be used with the same commandpool
                            index = uint32_invalid_id;
                            break;
                        }
                    }
                    else if ( uint32_invalid_id == tempIndex )
                    {
                        // Should not hit this...
                        MSG("tempIndex is invalid...");
                        assert( uint32_invalid_id != tempIndex );
                        break;
                    }
                    // Either indicies match or this is the first iteration
                    // Store the index and get the queue
                    index = tempIndex;
                    vkGetDeviceQueue( device, index, 0, tempQueue );
                    // TODO(klek): Add possibility to output the type of queue
                    MSG("Created device queue!");
                    // We found our queue, now break
                    break;
                }
                return index;
            }

            bool isPresentSupported( uint32_t queueFamilyIndex )
            {
                return core::getPresentSupported( queueFamilyIndex );
            }

            bool createCmdBuffer( VkDevice device, bool primary, VkCommandBuffer& buffer )
            {
                VkResult result { VK_SUCCESS };
                VkCommandBufferLevel level = ( primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY :
                                                         VK_COMMAND_BUFFER_LEVEL_SECONDARY );
                VkCommandBufferAllocateInfo info {
                    vulkan_utils::setCmdBufferCreateInfo( m_CmdPool, level, 1 )
                };

                result = vkAllocateCommandBuffers( device, &info, &buffer );
                if ( VK_SUCCESS != result )
                {
                    MSG("Failed to allocate command buffer...");
                    return false;
                }
                MSG("Command buffer allocated!");
                return true;
            }

            bool createSemaphore( VkDevice device, VkSemaphore& sem )
            {
                VkResult result { VK_SUCCESS };
                VkSemaphoreCreateInfo semInfo { vulkan_utils::setSemaphoreCreateInfo() };
                result = vkCreateSemaphore( device, &semInfo, nullptr, &sem );
                if ( VK_SUCCESS != result )
                {
                    MSG("Failed to create semaphore...");
                    return false;
                }
                MSG("Created semaphore!");
                return true;
            }

            bool createFence( VkDevice device, bool signalled, VkFence& fence )
            {
                VkResult result { VK_SUCCESS };
                VkFenceCreateInfo fenceInfo { vulkan_utils::setFenceCreateInfo( signalled ) };
                result = vkCreateFence( device, &fenceInfo, nullptr, &fence );
                if ( VK_SUCCESS != result )
                {
                    MSG("Failed to create fence...");
                    return false;
                }
                MSG("Created a fence!");
                return true;
            }

            VkCommandPool                   m_CmdPool { VK_NULL_HANDLE };
            VkQueue                         m_Queue { VK_NULL_HANDLE };
            bool                            m_PresentSupport { false };
            VkDevice                        m_Device{ VK_NULL_HANDLE };
            vulkan_frame                    m_Frames[ constants::FRAME_BUFFER_COUNT ]{ };
            uint32_t                        m_FrameIndex { 0 };
        };

        //************************************************************
        // Internal variables
        VkInstance                          instance{ VK_NULL_HANDLE };
        VkDebugUtilsMessengerEXT            debugMessenger{ VK_NULL_HANDLE };
        device_group                        deviceGroup;
        utils::vector<vulkan_queue_family>  queueFamilyIndices;
        vulkan_command                      gfxCmd{ };
        surface_collection                  surfaces{ };
        VkFormat                            deviceDepthFormat { VK_FORMAT_UNDEFINED };

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

        // This function finds the available queue families for the given
        // physical device and surface. This is important because we need
        // at least a graphics queue with presentation support in order
        // to display anything on the screen
        void getQueueFamilies( VkPhysicalDevice device, VkSurfaceKHR surface )
        {
            // This functions is only supposed to run once
            // Thus if the queueFamilyIndices vector is not zero
            // we should assert here
            assert( queueFamilyIndices.size() == 0 );
            VkResult result { VK_SUCCESS };

            // Setup the queues we want to find
            queueFamilyIndices.emplace_back( VK_QUEUE_GRAPHICS_BIT );

            // Get the number of queue families
            uint32_t queueFamCount { 0 };
            vkGetPhysicalDeviceQueueFamilyProperties( device,
                                                      &queueFamCount,
                                                      nullptr );

            // List all queue families
            utils::vector<VkQueueFamilyProperties> queueFamList( queueFamCount );
            vkGetPhysicalDeviceQueueFamilyProperties( device,
                                                      &queueFamCount,
                                                      queueFamList.data() );

            // For every queue we have in the vector we need
            // to look or a corresponding queue family
            for ( auto& queue : queueFamilyIndices )
            {
                // Get the queueType we are looking for
                VkQueueFlagBits type = queue.getQueueType();

                // Check if at least one of each desired queue is found
                uint32_t i { 0 };
                // for ( const auto& queueFam : queueFamList )
                for ( uint32_t i { 0 }; i < queueFamList.size(); i++ )
                {
                    // Get reference tot he current element
                    const VkQueueFamilyProperties& queueFam { queueFamList[i] };

                    // Check if queue family has at least one queue in this family
                    // We check for queue type here
                    if ( !( queueFam.queueCount > 0 ) ||
                         !( queueFam.queueFlags & type ) )
                    {
                        // If queue family is not valid, or the current
                        // queue isn't supported, we break and go next
                        break;
                    }

                    // Check if this queue family supports presentation
                    VkBool32 presentationSupport { false };
                    result = vkGetPhysicalDeviceSurfaceSupportKHR( device,
                                                                   i,
                                                                   surface,
                                                                   &presentationSupport );
                    // TODO(klek): Possibly change function name to
                    //             return a boolean...for now we assert
                    assert( VK_SUCCESS == result );
                    // Check if this queue family supports presentation
                    // (can be both graphics and presentation)
                    if ( queueFam.queueCount > 0 && presentationSupport )
                    {
                        // If presentation support is valid, then we
                        // Set the current slot
                        queue.setQueueFamilyIndex( i );
                        queue.setPresentationSupport( true );
                    }
                }
            }
        }

        // Helper function for isDeviceSuitable()
        bool isDeviceExtensionsSupported( VkPhysicalDevice device )
        {
            VkResult result { VK_SUCCESS };
            // Get number of extensions
            uint32_t extCount { 0 };
            result = vkEnumerateDeviceExtensionProperties( device,
                                                           nullptr,
                                                           &extCount,
                                                           nullptr );

            if ( VK_SUCCESS != result || 0 == extCount )
            {
                MSG("Could not create a list of extension properties...");
                return false;
            }

            // Create a list of extension properties based on count
            utils::vector<VkExtensionProperties> extensions( extCount );
            result = vkEnumerateDeviceExtensionProperties( device,
                                                           nullptr,
                                                           &extCount,
                                                           extensions.data() );
            if ( VK_SUCCESS != result )
            {
                MSG("Could not create a list of extension properties...");
                return false;
            }

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

        // Function to check if a device is suitable compared to our
        // needs
        // TODO(klek): Update this function for our needs
        bool isDeviceSuitable( VkPhysicalDevice device, VkSurfaceKHR surface )
        {
            // Populate the queue indices
            getQueueFamilies( device, surface );

            bool extSupported { isDeviceExtensionsSupported( device ) };

            bool swapchainValid { false };
            if ( extSupported )
            {
                swapchain_details swapchainDetails;
                bool result = getSwapchainDetails( device,
                                                   surface,
                                                   swapchainDetails );
                assert( result );
                swapchainValid = !swapchainDetails.formats.empty() &&
                                 !swapchainDetails.presentationModes.empty();
            }

            bool queueFamiliesSupported { true };
            for ( auto& queue : queueFamilyIndices )
            {
                queueFamiliesSupported &= queue.isValid();
            }

            return ( queueFamiliesSupported &&
                     extSupported           &&
                     swapchainValid            );
        }

        // NOTE(klek): This function is currently just an example
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

        // Function to find a suitable physical device for the provided
        // surface. This physical device also has to fulfill other
        // requirements such as being able to support certain features
        // in hardware and being able to support certain queues
        bool findPhysicalDevice( VkSurfaceKHR surface )
        {
            // Get number of physical devices that support vulkan
            uint32_t deviceCount{ 0 };
            VkResult result{ VK_SUCCESS };
            result = vkEnumeratePhysicalDevices( instance,
                                                 &deviceCount,
                                                 nullptr );
            assert( deviceCount > 0 );
            if ( VK_SUCCESS != result || 0 == deviceCount )
            {
                MSG("Failed to find GPUs with vulkan support...");
                return false;
            }

            // Get all the physical devices that support vulkan
            utils::vector<VkPhysicalDevice> devices{ deviceCount };
            result = vkEnumeratePhysicalDevices( instance, &deviceCount, devices.data() );
            if ( VK_SUCCESS != result )
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
                    // Set the internal variable here and break
                    deviceGroup.physicalDevice = device;
                    break;
                }
            }
            // Add assert for the device we just added
            assert( deviceGroup.physicalDevice );

            // Check if the device we just added is valid, ie not nullptr
            if ( VK_NULL_HANDLE == deviceGroup.physicalDevice )
            {
                MSG(" Failed to find suitable physical device...");
                return false;
            }

            MSG("Physcial device found successfully!");

            return true;
        }

        // Function to create a logical device based on a physical
        // device which by now should be populated in the deviceGroup
        // variable
        bool createLogicalDevice( void )
        {
            VkResult result { VK_SUCCESS };
            // Setup queue creation information
            utils::vector<VkDeviceQueueCreateInfo> infos { };

            // Queues the logical device needs to create and info to do so
            for ( auto& queue : queueFamilyIndices )
            {
                uint32_t queueFamilyIndex = queue.getQueueFamilyIndex();
                VkDeviceQueueCreateInfo info {
                    vulkan_utils::setQueueCreateInfo( queueFamilyIndex,
                                                      1,
                                                      0 )
                };
                infos.push_back( info );
            }

            // Information to crete logical device
            // A physical device might have features, that the logical
            // device will be using, this is currently not specified
            // TODO(klek): Specify the device features we need
            // VkPhysicalDeviceFeatures deviceFeat { };
            VkDeviceCreateInfo info { vulkan_utils::setDeviceCreateInfo( infos,
                                                                         deviceExt ) };

            result = vkCreateDevice( deviceGroup.physicalDevice, &info, nullptr, &deviceGroup.logicalDevice );
            if ( VK_SUCCESS != result )
            {
                MSG("Failed to create a logical device...");
                return false;
            }

            MSG("Logical device created successfully!");

            return true;
        }

        // Function that creates a vulkan instance if possible
        // This function also creates a debug messenger if validation
        // layers has been enabled
        bool createInstance( void )
        {
            // Determine which adapter (i.e graphics card) to use
            // Determine what is the maximum feature level that is supported
            // Create a vulkan device (this is a virtual adapter)

            // If we already have an instance, then we return false
            if ( instance )
            {
                return false;
            }

            // Check if validation layers should be used and are
            // available
            if ( ENABLE_VALIDATION_LAYERS && !isValidationLayerSupported() )
            {
                // TODO(klek): Set this to an error message
                MSG("Validation layers requested, but not available...");
                return false;
            }

            // Set up the application info struct
            VkApplicationInfo appInfo{ vulkan_utils::setApplicationInfo( VK_MAKE_VERSION( 1, 0, 0 ),
                                                                         VK_MAKE_VERSION( 1, 0, 0 ),
                                                                         VK_API_VERSION_1_2 ) };

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
                return false;
            }

            // Set up the creation info struct
            VkInstanceCreateInfo createInfo{ vulkan_utils::setInstanceCreateInfo( appInfo, extensions ) };

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
            if ( VK_SUCCESS != result )
            {
                // TODO(klek): Set this to an error message
                MSG("Failed to create vulkan instance...");
                return false;
            }

            MSG("Vulkan instance has been created!");

            // Add the debug messenger if validation layers are enabled
            if ( ENABLE_VALIDATION_LAYERS )
            {
                VkDebugUtilsMessengerCreateInfoEXT debugInfo{ };
                setupDebugCreateInfo( debugInfo );
                result = createDebugUtilsMessengerExt( instance,
                                                    &debugInfo,
                                                    nullptr,
                                                    &debugMessenger );
                if ( VK_SUCCESS != result )
                {
                    MSG("Failed to setup vulkan debug messenger...");
                    return false;
                }

                MSG("Vulkan validation layer has been created!");
            }

            return true;
        }

        // Failed inititialize function that is called when something
        // fails during initialization. This should then result in a
        // shutdown
        bool failedInit( void )
        {
            shutdown();
            return false;
        }
    } // Anonymous namespace, ie only for use in this cpp-file

    //****************************************************************
    // Create and initialization functions
    
    // Initialize function that creates an instance and loads
    // the respective submodules
    // This function should only be called once, directly after
    // choosing and starting an API (vulkan in this case)
    bool initialize( void )
    {
        if ( instance )
        {
            return failedInit();
        }

        // Initialize volk
        // if ( VK_SUCCESS != volkInitialize() )
        // {
        //     return failedInit();
        // }

        // Try to create a vulkan instance
        if ( !createInstance() )
        {
            return failedInit();
        }

        // Volk load instance
        //volkLoadInstance( instance );

        //************************************************************
        // Submodule initialization
        // TODO(klek): Add other submodules here
        // if ( !shaders::initialize() ||
        //      !content::init()          )
        if ( !shaders::initialize() )
        {
            return failedInit();
        }
        //************************************************************

        MSG("Vulkan has been successfully initialized!");

        return true;
    }

    // Shutdown function, which is the opposite to initialize(). This
    // function shuts down the resources that initialize has been
    // loading. When this function has run, all other vulkan related
    // resources should be unloaded and released.
    void shutdown( void )
    {
        // Release the command
        gfxCmd.release( );

        //************************************************************
        // Shutdown submodules
        // Content submodule
        content::shutdown( );
        // Shaders submodule
        shaders::shutdown( );
        // TODO(klek): Add the graphics pipeline
        //pipeline::shutdown( deviceGroup.logicalDevice );
        // TODO(klek): Add renderpass 
        //************************************************************

        // TODO(klek): Destroy the device we have created
        // NOTE(klek): This device was created after a surface!
        //             Shouldn't this be in surface remove?
        // This is here because we only allow 1 device to be created
        // and this device should live until we shutdown the api
        vkDestroyDevice( deviceGroup.logicalDevice, nullptr );
        deviceGroup.logicalDevice = VK_NULL_HANDLE;
        deviceGroup.physicalDevice = VK_NULL_HANDLE;
        queueFamilyIndices.clear();

        // TODO(klek): Destroy any validation layers we created
        if ( ENABLE_VALIDATION_LAYERS )
        {
            // Destroy the debug messenger
            destroyDebugUtilsMessengerExt( instance, debugMessenger, nullptr );
        }

        // Destroying the instance
        vkDestroyInstance( instance, nullptr );
        instance = VK_NULL_HANDLE;
    }

    // Function to find a physical device and the create a logical
    // device based on the physical device.
    // This means that there will be certain requirements put on the
    // physical device, that must be fulfilled
    bool createDevice( VkSurfaceKHR surface )
    {
        // A device should only be create once
        if ( deviceGroup.logicalDevice || deviceGroup.physicalDevice ) 
        {
            return true;
        }
        assert( !deviceGroup.logicalDevice && !deviceGroup.physicalDevice );

        return ( findPhysicalDevice( surface ) && createLogicalDevice() );
    }

    // This function populates the local variable gfxCmd with proper
    // data instead of the default construction that has already
    // happened
    // NOTE(klek): It is important that the default initialization
    //             via default constructor does not allocate anything
    //             to the gfxCmd, otherwise this "re-assignment" will
    //             result in a memory leak.
    bool createGraphicsCommand( uint32_t swapchainBufferSize )
    {
        // This should only be created once
        if ( gfxCmd.getCmdPool() )
        {
            return true;
        }
        assert( !gfxCmd.getCmdPool() );

        // Create a new class in place of the default constructed one
        uint8_t type = VULKAN_GRAPHICS_QUEUE | VULKAN_PRESENT_SUPPORT;
        new ( &gfxCmd ) vulkan_command( deviceGroup.logicalDevice, 
                                        type );
        // Check that it got created
        if ( !gfxCmd.getCmdPool() )
        {
            return false;
        }

        return true;
    }

    //****************************************************************
    // Helper functions

    // This function detects the depthformat that is supported by the
    // provided physical device. If it finds a format, it sets the
    // local variable deviceDepthFormat to this format and returns
    // true, otherwise it returns false.
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

    // This function tries to find a memory index for the provided
    // type and flags. If an index is found, it is returne in the
    // index-variable and the functions returns true.
    bool findMemoryIndex( uint32_t type, uint32_t flags, uint32_t& index )
    {
        VkPhysicalDeviceMemoryProperties prop;
        vkGetPhysicalDeviceMemoryProperties( deviceGroup.physicalDevice, &prop );

        for ( uint32_t i { 0 }; i < prop.memoryTypeCount; i++ )
        {
            if ( ( type & BIT( i ) ) && 
                 ( ( prop.memoryTypes[ i ].propertyFlags & flags ) == flags ) )
            {
                index = i;
                return true;
            }
        }

        MSG("Cannot find memory type...");
        return false;
    }

    // Return a copy of the current graphics family index.
    // NOTE(klek): Shouldn't this be a const reference instead?
    uint32_t getGraphicsFamilyQueueIndex( void )
    {
        // This now becomes a loop
        for ( auto& queue : queueFamilyIndices )
        {
            if ( queue.getQueueType() == VK_QUEUE_GRAPHICS_BIT )
            {
                return queue.getQueueFamilyIndex();
            }
        }
        return uint32_invalid_id;
    }

    // Return a copy of the current presentation family index.
    // NOTE(klek): Shouldn't this be a const reference instead?
    uint32_t getPresentationFamilyQueueIndex( void )
    {
        // TODO(klek): This only supports graphics queus as
        //             presentation at the moment. Consider improving
        // This now becomes a loop
        for ( auto& queue : queueFamilyIndices )
        {
            if ( ( queue.getQueueType() == VK_QUEUE_GRAPHICS_BIT ) &&
                 ( queue.isPresentationSupported() ) )
            {
                return queue.getQueueFamilyIndex();
            }
        }
        return uint32_invalid_id;
    }

    // Return a copy of the current copy family index.
    // NOTE(klek): Shouldn't this be a const reference instead?
    uint32_t getCopyFamilyQueueIndex( void )
    {
        // TODO(klek): Implement this
        return uint32_invalid_id;
    }

    // Return a copy of the current compute family index.
    // NOTE(klek): Shouldn't this be a const reference instead?
    uint32_t getComputeFamilyQueueIndex( void )
    {
        // TODO(klek): Implement this
        return uint32_invalid_id;
    }

    // Return true if the index requested does support presenting
    bool getPresentSupported( const uint32_t index )
    {
        // TODO(klek): Consider if we need to check all the different
        //             queues in queueFamilyIndices...
        // NOTE(klek): It should be enough with one, since if the family
        //             that the queue belongs too does support
        //             presentation, so should the queue
        for ( auto& queue : queueFamilyIndices )
        {
            if ( ( queue.getQueueFamilyIndex() == index ) &&
                 ( queue.isPresentationSupported() ) )
            {
                return true;
            }
        }
        return false;
    }

    // Return a copy of the current deviceDepthFormat.
    // NOTE(klek): Shouldn't this be a const reference instead?
    VkFormat getDepthFormat( void )
    {
        return deviceDepthFormat;
    }

    // Return a copy of the currently selected physical device
    // NOTE(klek): Shouldn't this be a const reference instead?
    VkPhysicalDevice getPhysicalDevice( void )
    {
        return deviceGroup.physicalDevice;
    }

    // Return a copy of the current logicalDevice.
    // NOTE(klek): Shouldn't this be a const reference instead?
    VkDevice getLogicalDevice( void )
    {
        return deviceGroup.logicalDevice;
    }

    // Return a copy of the current instance.
    // NOTE(klek): Shouldn't this be a const reference instead?
    VkInstance getInstance( void )
    {
        return instance;
    }

    // Return the current frame index
    uint32_t getCurrentFrameIndex( void )
    {
        return gfxCmd.getCurrentFrame();
    }

    //****************************************************************
    // Low-level functions for vulkan surfaces
    // The function-pointer table in the high-level renderer should be
    // initialized to these functions when the API is choosen

    // Adds a new entry (or reuses an old and unused) to the
    // surface_collection variable surfaces.
    // If the new entry returns a valid id, we then call the create-
    // function from the vulkan_surfaces class to actually create the
    // new surface
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

    // Here we simply remove a surface, which in turn will remove
    // the actual surface along with its position in the
    // surface_collection variable surfaces
    void removeSurface( surface_id id )
    {
        assert( id::isValid( id ) );
        surfaces.remove( id );
    }

    // Function to resize the surface
    void resizeSurface( surface_id id, uint32_t width, uint32_t height )
    {
        assert( id::isValid( id ) );
        // TODO(klek): Add usage of width and height for resizing
        surfaces[ id ].setResizeRequested();
    }

    // Function to indicate that the window that the surface
    // belongs to is either minimized or not
    void minimizeSurface( surface_id id, bool isMinimized )
    {
        assert( id::isValid( id ) );
        if ( isMinimized )
        {
            surfaces[ id ].setMinimized( );
        }
        else
        {
            surfaces[ id ].clearMinimized( );
        }
    }

    // Function to get the current surface width
    uint32_t getSurfaceWidth( surface_id id )
    {
        assert( id::isValid( id ) );
        return surfaces[ id ].getWidth();
    }

    // Function to get the current surface height
    uint32_t getSurfaceHeight( surface_id id )
    {
        assert( id::isValid( id ) );
        return surfaces[ id ].getHeight();
    }
    
    // Function that handles the rendering to the provided surface.
    // This will likely also need another variable containing
    // information about what to render
    void renderSurface( surface_id id /*, frame_info info*/ )
    {
        // Begin frame, sets up the current frame in the
        // gfxCmd.vulkan_frame buffer
        // Waits for the fence
        // Resets the fence
        // Acquires the next image via vkAcquireNextImageKHR
        // Checks that the swapchain is good

        // Then we start to prepare the frame
        // This includes allocating a new framebuffer
        // Resetting and beginning a new commandbuffer that also
        // records the following commands
        // Setting up barriers if queues are not the same
        // Starting a renderpass
        // Binding a pipeline for this render
        // Setting viewport and scissor
        // Binding vertexbuffer for this render
        // Binding descriptorsets for this render
        // Issue actual draw call
        // Ending the renderpass 
        // Again setting up barriers if queues are not the same
        // Ending the recording of commands via ending the commandbuffer

        // Finally we end the frame by
        // First submitting the current commandbuffer to the
        // queue via vkQueueSubmit
        // And then presenting the results via vkQueuePresentKHR
        // The last thing we do is calculate the next frameIndex

        // So, here we go...
        // Get a reference to the current surface
        vulkan_surface& surface { surfaces[ id ] };
        // First check if the current surface is minimized, if it is
        // then we skip it
        if ( surface.isMinimized() )
        {
            return;
        }
        // Start the frame
        // NOTE(klek): Consider setting this up into an if-statement
        if ( gfxCmd.beginFrame( ) )
        {
            // Get a handle to the current command buffer
            const VkCommandBuffer& cmdBuffer { gfxCmd.getCurrentCmdBuffer() };
            // Get the current frameIndex
            uint32_t frameIndex { getCurrentFrameIndex() };
            static uint32_t frame;

            // Clean out old data from

            // Get the next image in the swapchain
            // if ( !surface.getNextImage( gfxCmd.getCurrentImageSemaphore(),
            //                             nullptr ) )
            // {
            //     return;
            // }
            surface.getNextImage( gfxCmd.getCurrentImageSemaphore(),
                                  nullptr );

            // Create a framebuffer
            //surface.generateFrameBuffer( );
            // Copy data
            content::copyObjects( cmdBuffer );

            // Setting clear color
            // float flash = math::maths_abs( math::maths_sin( frameIndex / 120.0f ) );
            surface.setClearColor( { 0.0f, 0.5f, 0.0f, 0.0f } );

            // Set the renderarea
            surface.setRenderArea( { 0, 0, surface.getWidth(), surface.getHeight() } );

            // Start renderpass
            surface.beginRenderPass( cmdBuffer );

            // Setup viewport and scissor
            VkViewport viewport { };
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = static_cast<float>( surface.getWidth() );
            viewport.height = static_cast<float>( surface.getHeight() );
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            vkCmdSetViewport( cmdBuffer, 0, 1, &viewport );

            VkRect2D scissor { };
            scissor.offset.x = 0;
            scissor.offset.y = 0;
            scissor.extent.width = surface.getWidth();
            scissor.extent.height = surface.getHeight();
            vkCmdSetScissor( cmdBuffer, 0, 1, &scissor );

            // Transition image to write optimal
            // vulkan_utils::transitionImage( cmdBuffer,
            //                                surface.getCurrentImage(),
            //                                VK_IMAGE_LAYOUT_UNDEFINED,
            //                                VK_IMAGE_LAYOUT_GENERAL );

            // Draw objects
            content::drawObjects( cmdBuffer,
                                  {
                                    (float)surface.getWidth(),
                                    (float)surface.getHeight(),
                                    (float)frame++
                                  } );

            // Transition image to present optimal
            // vulkan_utils::transitionImage( cmdBuffer,
            //                                surface.getCurrentImage(),
            //                                VK_IMAGE_LAYOUT_GENERAL,
            //                                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR );

            // End the current renderpass
            surface.endRenderPass( cmdBuffer );

            // Can we end the frame and present it??
            // if ( gfxCmd.endFrame( surface ) )
            // {
            //     return;
            // }
            // NOTE(klek): Actually at the moment we don't care if the
            //             endFrame() worked or not, because we just
            //             want to end it
            gfxCmd.endFrame( surface );
        }
        // else
        // {
        //     // Well, we failed to start the frame...
        //     // Should we handle this??
        // }

    }

} // namespace muggy::graphics::vulkan::core
