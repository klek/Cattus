//********************************************************************
//  File:    vulkanSurface.cpp
//  Date:    Sun, 17 Mar 2024: 19:07
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if USE_OLD_SURFACE
#include "oldVulkanSurface.h"
#include "vulkanCore.h"
#include "vulkanRenderPass.h"
#include "vulkanResources.h"
#include "vulkanGraphicsPipeline.h"

namespace muggy::graphics::vulkan
{
    namespace 
    {
        VkSurfaceFormatKHR chooseBestSurfaceFormat( const utils::vector<VkSurfaceFormatKHR>& formats )
        {
            // VK_FORMAT_UNDEFINED means all formats are available
            if ( ( formats.size() == 1 ) && ( formats[ 0 ].format == VK_FORMAT_UNDEFINED ) )
            {
                return { VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
            }

            // If not all formats are available, choose the best available
            for ( const auto& format : formats )
            {
                // Check for RGBA (first) or BGRA (second)
                if ( ( format.format == VK_FORMAT_R8G8B8A8_UNORM ||
                       format.format == VK_FORMAT_B8G8R8A8_UNORM   ) &&
                     ( format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR ) )
                {
                    return format;
                }
            }

            // If optimal format is not found, just return the first available one
            return formats[0];
        }

        VkPresentModeKHR chooseBestPresentationMode( const utils::vector<VkPresentModeKHR> modes )
        {
            // NOTE(klek): VK_PRESENT_MODE_MAILBOX_KHR is preferred, as
            //             it always uses the next newest image available,
            //             and gets rid of any that are older.
            //             This results in optimal response time, with no
            //             tearing.
            //             VK_PRESENT_MODE_FIFO_KHR is the backup, as 
            //             vulkan spec requires it to be available. It also
            //             does not produce tearing, but may be somewhat 
            //             less responsive as each image is displayed no
            //             matter how old it is.
            for ( const auto& mode : modes )
            {
                if ( mode == VK_PRESENT_MODE_MAILBOX_KHR )
                {
                    return mode;
                }
            }

            return VK_PRESENT_MODE_FIFO_KHR;
        }

        VkExtent2D chooseBestSwapExtent( const VkSurfaceCapabilitiesKHR& capabilities,
                                         uint32_t width,
                                         uint32_t height )
        {
            // NOTE(klek): If the currentExtent is at its numeric limits,
            //             extent can vary in size from the window.
            //             Otherwise, it is the size of the window.
            if ( capabilities.currentExtent.width != uint32_invalid_id )
            {
                return capabilities.currentExtent;
            }
            // We set the extent manually
            else
            {
                VkExtent2D extent {};
                extent.width = width;
                extent.height = height;

                // Clamp the extent to be within the boundaries of surface min/max
                extent.width = MUGGY_MAX( capabilities.minImageExtent.width, MUGGY_MIN( capabilities.maxImageExtent.width, extent.width ) );
                extent.height = MUGGY_MAX( capabilities.minImageExtent.height, MUGGY_MIN( capabilities.maxImageExtent.height, extent.height ) );

                return extent;
            }
        }

        VkImageUsageFlags getImageUsageFlags( const VkSurfaceCapabilitiesKHR& capabilities )
        {
            // Color attachment flag must always be supported
            // Check for transfer destination bit to also support image
            // clear operations
            if ( capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT )
            {
                return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            }
            MSG("VK_IMAGE_USAGE_TRANSFER_DST image usage is not supported bu the swap chain...");
            // Transfer destination not available
            return VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM;
        }

        bool createImageView( VkImage image, VkFormat format,
                                     VkImageAspectFlags aspectFlags,
                                     VkImageView& imageView )
        {
            VkImageViewCreateInfo info { };
            info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            info.pNext = nullptr;
            info.flags = 0;
            info.image = image;
            info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            info.format = format;
            info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            // NOTE(klek): Subresources allow the view to view only part 
            //             of the image
            // Which part of image to view
            info.subresourceRange.aspectMask = aspectFlags;
            // Start mipmap level to view from
            info.subresourceRange.baseMipLevel = 0;
            // Number of mipmap levels to view
            info.subresourceRange.levelCount = 1;
            // Start array level to view from
            info.subresourceRange.baseArrayLayer = 0;
            // Number of array levels to view
            info.subresourceRange.layerCount = 1;

            VkResult result { VK_SUCCESS };
            result = vkCreateImageView( core::getLogicalDevice(), &info, nullptr, &imageView );
            if ( VK_SUCCESS != result )
            {
                MSG("Failed to create image view...");
                return false;
            }

            return true;
        }

    } // namespace anonymous
    
    void vulkan_surface::create( VkInstance instance )
    {
        // Creating a vulkan surface happens before creating a device
        // because a vulkan device needs information from the surface
        // creation
        bool result { true };
        // TODO(klek): Add proper error handling
        result = createSurface( instance );
        assert( result );
        // TODO(klek): Add proper error handling
        result = core::createDevice( m_Surface );
        assert( result );
        // TODO(klek): Add proper error handling
        result = createSwapchain( );
        assert( result );
        // TODO(klek): Add proper error handling
        result = createRenderpass( );
        assert( result );
        // TODO(klek): Add proper error handling
        result = pipeline::createGraphicsPipeline( core::getLogicalDevice(),
                                                  m_Renderpass.renderPass,
                                                  m_GraphicsPipeline,
                                                  m_PipelineLayout );
        assert ( result );
        // TODO(klek): Add proper error handling
        result = reCreateFramebuffers( );
        assert( result );
        // TODO(klek): Add proper error handling
        result = core::createGraphicsCommand( (uint32_t)m_Swapchain.images.size() );
        assert( result );
    }

    void vulkan_surface::release( void )
    {
        vkDeviceWaitIdle( core::getLogicalDevice() );
        for ( uint32_t i {0 }; i < m_Swapchain.images.size(); i++ )
        {
            destroyFrameBuffer( core::getLogicalDevice(), m_FrameBuffers[ i ] );
        }
        cleanSwapchain();

        //pipeline::shutdown( core::getLogicalDevice() );
        pipeline::destroyGraphicsPipeline( core::getLogicalDevice(),
                                           m_GraphicsPipeline,
                                           m_PipelineLayout );
        renderpass::destroyRenderPass( core::getLogicalDevice(), m_Renderpass );

        // NOTE(klek): Destruction of device happens when we shutdown
        //             the api in core::shutdown()!
        // Destroy this surface
        vkDestroySurfaceKHR( core::getInstance(), m_Surface, nullptr );
    }

    void vulkan_surface::present( VkSemaphore imageAvailable, 
                                  VkSemaphore renderFinished, 
                                  VkFence fence, 
                                  VkQueue presentationQueue )
    {
        // Present image
        VkPresentInfoKHR info { };
        info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &renderFinished;
        info.swapchainCount = 1;
        info.pSwapchains = &m_Swapchain.swapchain;
        info.pImageIndices = &m_ImageIndex;
        VkResult result { VK_SUCCESS };
        result = vkQueuePresentKHR( presentationQueue, &info );
        if ( result == VK_ERROR_OUT_OF_DATE_KHR || 
             result == VK_SUBOPTIMAL_KHR || 
             m_IsFrameBufferResized )
        {
            m_IsFrameBufferResized = false;
            reCreateSwapchain();
        }
        else if ( result != VK_SUCCESS )
        {
            MSG("Failed to present swapchain...");
        }

        m_FrameIndex = ( m_FrameIndex + 1 ) % m_Swapchain.images.size();
    }

    void vulkan_surface::resize( void )
    {
        m_IsFrameBufferResized = true;
    }
    
    bool vulkan_surface::reCreateSwapchain()
    {
        vkDeviceWaitIdle( core::getLogicalDevice() );

        m_IsRecreating = true;

        cleanSwapchain();
        m_Swapchain.images.clear();
        if ( !createSwapchain() )
        {
            return false;
        }
        if ( !reCreateFramebuffers() )
        {
            return false;
        }

        m_IsRecreating = false;
        m_IsFrameBufferResized = false;
        
        return true;
    }

    bool vulkan_surface::nextImageIndex( VkSemaphore imageAvailable, 
                                         VkFence fence, 
                                         uint64_t timeout )
    {
        // Get next image
        VkResult result { VK_SUCCESS };
        result = vkAcquireNextImageKHR( core::getLogicalDevice(),
                                        m_Swapchain.swapchain,
                                        timeout,
                                        imageAvailable,
                                        fence, 
                                        &m_ImageIndex );
        if ( VK_ERROR_OUT_OF_DATE_KHR == result )
        {
            reCreateSwapchain();
            return false;
        }
        else if ( VK_SUCCESS != result && VK_SUBOPTIMAL_KHR != result )
        {
            MSG("Failed to aquire swapchain image...");
            return false;
        }

        return true;
    }

    // This function contains platform specific code due to that
    // surfaces are created based on the window system in use
    bool vulkan_surface::createSurface( VkInstance instance )
    {
#if ( defined( GLFW ) || defined( GLFW3 ) )
        // NOTE(klek): GLFW has implementation for this for all 
        //             platforms, so we first try with GLFW and
        //             if it fails, we try to do it manually
        VkResult result { VK_SUCCESS };
        result = glfwCreateWindowSurface( instance, 
                                          (GLFWwindow*)m_Window.handle(), 
                                          nullptr, 
                                          &m_Surface );
        if ( VK_SUCCESS != result )
        {
            MSG("GLFW failed to create surface...");

            // NOTE(klek): Trying to create a surface manually
            //MUGGY_VULKAN_CREATE_SURFACE( instance, m_Window, m_Surface );
            #if defined( _WIN64 )
            {
                VkWin32SurfaceCreateInfoKHR createInfo {};
                createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
                createInfo.hwnd = glfwGetWin32Window( (GLFWwindow*)m_Window.handle() );
                createInfo.hinstance = GetModuleHandle( nullptr );

                result = vkCreateWin32SurfaceKHR( instance,
                                                  &createInfo,
                                                  nullptr,
                                                  &m_Surface );
                if ( VK_SUCCESS != result )
                {
                    MSG("Failed to manually create a surface...");
                    return false;
                }
            }
            #elif defined( __linux__ )
            {
                VkXlibSurfaceCreateInfoKHR createInfo {};
                createInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
                createInfo.dpy = glfwGetX11Display( /*window.handle()*/ );
                //createInfo.window = *(Window*)m_Window.handle();
                createInfo.window = glfwGetX11Window( (GLFWwindow*)m_Window.handle() );

                result = vkCreateXlibSurfaceKHR( instance,
                                                 &createInfo,
                                                 nullptr,
                                                 &m_Surface );
                if ( VK_SUCCESS != result )
                {
                    MSG("Failed to manually create a surface...");
                    return false;
                }
            }
            #endif
        }
#elif defined( _WIN64 )
        VkWin32SurfaceCreateInfoKHR createInfo { };
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hinstance = GetModuleHandle( NULL );
        createInfo.hwnd = (HWND)m_Window.getHandle;
        VkResult result { VK_SUCCESS };
        result = vkCreateWin32SurfaceKHR( instance,
                                          &createInfo,
                                          nullptr,
                                          &surface );
        if ( VK_SUCCESS != result )
        {
            MSG("Failed to create a surface...");
            return false;
        }
#elif defined( __linux__ )
        VkXlibSurfaceCreateInfoKHR createInfo { };
        createInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
        createInfo.dpy = /*TO BE IMPLEMENTED*/;
        createInfo.window = *(Window)m_Window.handle();
        VkResult result { VK_SUCCESS };
        result = vkCreateXlibSurfaceKHR( instance,
                                         &createInfo,
                                         nullptr,
                                         &surface );
        if ( VK_SUCCESS != result )
        {
            MSG("Failed to create a surface...");
            return false;
        }
#endif
        return true;
    }

    bool vulkan_surface::createRenderpass( void )
    {
         return renderpass::createRenderPass( core::getLogicalDevice(),
                                              m_Swapchain.imageFormat,
                                              core::getDepthFormat(),
                                              { 0, 0, m_Window.getWidth(), m_Window.getHeight() },
                                              { 0.0f, 0.0f, 0.0f, 0.0f },
                                              1.0f,
                                              0,
                                              m_Renderpass );
    }

    bool vulkan_surface::createSwapchain( void )
    {
        // We pick the best settings for the swapchain based on the
        // swapchain details from the physical device
        bool res = getSwapchainDetails( core::getPhysicalDevice(), m_Surface, m_Swapchain.details );
        assert( res );

        // Find optimal surface values for the swapchain
        VkSurfaceFormatKHR format { chooseBestSurfaceFormat( m_Swapchain.details.formats ) };
        VkPresentModeKHR mode { chooseBestPresentationMode( m_Swapchain.details.presentationModes ) };
        VkExtent2D extent { chooseBestSwapExtent( m_Swapchain.details.surfaceCapabilities, 
                                                  m_Window.getWidth(), 
                                                  m_Window.getHeight() ) };
        VkImageUsageFlags usageFlags { getImageUsageFlags( m_Swapchain.details.surfaceCapabilities ) };

        // Check that the desired image usage flags is valid
        if ( VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM == usageFlags )
        {
            return false;
        }

        uint32_t imagesInFlight = m_Swapchain.details.surfaceCapabilities.minImageCount + 1;
        // NOTE(klek): At this point, in a typical situation, imagesInFlight 
        //             will be at least 2, allowing us to use triple buffer.
        //             However, there will be a rare occasion that triple
        //             buffering is not supported. If that is the case, we
        //             will need to set imagesInFlight to the max image
        //             count supported. If maxImageCount == 0, there is no
        //             max limit
        if ( ( m_Swapchain.details.surfaceCapabilities.maxImageCount > 0 ) &&
             ( m_Swapchain.details.surfaceCapabilities.maxImageCount < imagesInFlight ) )
        {
            imagesInFlight = m_Swapchain.details.surfaceCapabilities.maxImageCount;
        }

        // Create the swapchain info
        VkSwapchainCreateInfoKHR info { };
        info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        info.pNext = nullptr;
        info.flags = 0;
        info.surface = m_Surface;
        info.minImageCount = imagesInFlight;
        info.imageFormat = format.format;
        info.imageColorSpace = format.colorSpace;
        info.imageExtent = extent;
        // Number of layers for each image in swapchain
        info.imageArrayLayers = 1;
        // What attachment each image will be used as
        info.imageUsage = usageFlags;
        // If graphics and presentation queue family indices are different, 
        // the swapchain must let images be shared between families
        if ( core::getGraphicsFamilyQueueIndex() != core::getPresentationFamilyQueueIndex() )
        {
            uint32_t indices[] 
            {
                (uint32_t)core::getGraphicsFamilyQueueIndex(),
                (uint32_t)core::getPresentationFamilyQueueIndex()
            };
            info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            info.queueFamilyIndexCount = 2;
            info.pQueueFamilyIndices = indices;
        }
        else
        {
            info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            info.queueFamilyIndexCount = 0;
            info.pQueueFamilyIndices = nullptr;
        }
        // Transform to perform on swapchain images
        info.preTransform = m_Swapchain.details.surfaceCapabilities.currentTransform;
        // How to handle blending images with external graphics (such as
        // other windows, OS UIs etc)
        info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        info.presentMode = mode;
        // Clip parts of images not in view? (ie offscreen, behind
        // another window etc)
        info.clipped = VK_TRUE;

        // NOTE(klek): If old swapchain has been destroyed and this one
        //             is replacing it, you can use this to link the old
        //             one to quickly hand over responsibilities. 
        //             Not using this feature for now
        info.oldSwapchain = VK_NULL_HANDLE;

        // Chech to make sure that the given surface is supported by
        // the device for each queue
        // TODO(klek): Extend this when using compute and transfer
        //             queues
        VkBool32 supported { false };
        VkResult result { VK_SUCCESS };
        result = vkGetPhysicalDeviceSurfaceSupportKHR( core::getPhysicalDevice(),
                                                       core::getGraphicsFamilyQueueIndex(),
                                                       m_Surface,
                                                       &supported );
        if ( ( VK_SUCCESS != result ) || ( VK_FALSE == supported ) )
        {
            MSG("Physical device does not support graphics family queue for this surface...");
            return false;
        }
        result = vkGetPhysicalDeviceSurfaceSupportKHR( core::getPhysicalDevice(),
                                                       core::getPresentationFamilyQueueIndex(),
                                                       m_Surface,
                                                       &supported );
        if ( ( VK_SUCCESS != result ) || ( VK_FALSE == supported ) )
        {
            MSG("Physical device does not support presentation family queue for this surface...");
            return false;
        }

        result = vkCreateSwapchainKHR( core::getLogicalDevice(), 
                                       &info, 
                                       nullptr, 
                                       &m_Swapchain.swapchain );
        if ( VK_SUCCESS != result )
        {
            MSG("Failed to create swapchain...");
            return false;
        }

        m_Swapchain.imageFormat = format.format;
        m_Swapchain.extent = extent;

        // Get swapchain images and image views
        uint32_t imageCount { 0 };
        result = vkGetSwapchainImagesKHR( core::getLogicalDevice(),
                                          m_Swapchain.swapchain,
                                          &imageCount,
                                          nullptr );
        if ( ( VK_SUCCESS != result ) || ( imageCount == 0 ) )
        {
            MSG("Failed to get the number of swap chain images...");
            return false;
        }
        utils::vector<VkImage> images( imageCount );
        result = vkGetSwapchainImagesKHR( core::getLogicalDevice(),
                                          m_Swapchain.swapchain,
                                          &imageCount,
                                          images.data() );
        if ( VK_SUCCESS != result )
        {
            MSG("Failed to get swap chain images...");
            return false;
        }

        for ( const auto& image : images )
        {
            swapchain_image temp { };
            temp.image = image;
            if ( !createImageView( image,
                                   m_Swapchain.imageFormat,
                                   VK_IMAGE_ASPECT_COLOR_BIT,
                                   temp.imageView ) )
            {
                // TODO(klek): Handle error here!!
            }
            m_Swapchain.images.push_back( temp );
        }

        if ( !core::detectDepthFormat( core::getPhysicalDevice() ) )
        {
            MSG("Failed to find a supported depth format...");
            return false;
        }

        // Create depthbuffer image and view
        image_init_info imageInfo { };
        imageInfo.device = core::getLogicalDevice();
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.width = m_Swapchain.extent.width;
        imageInfo.height = m_Swapchain.extent.height;
        imageInfo.format = core::getDepthFormat();
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageInfo.memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        imageInfo.createView = true;
        imageInfo.viewAspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;

        if ( !createImage( &imageInfo, m_Swapchain.depthAttachment ) )
        {
            return false;
        }

        MSG("Swapchain created successfully!");

        return true;
    }

    // NOTE(klek): Currently we are creating a framebuffer for each
    //             image view in the swap chain
    //             Perhaps, this isn't what we want to do at a later
    //             date (due to performance in the render loop), but
    //             currently this simplifies the code for the
    //             render loop
    bool vulkan_surface::reCreateFramebuffers( void )
    {
        // Why are we resizing before we destroy the current?
        if( m_FrameBuffers.size() > 0 )
        {
            // This means that we have old data...
            // Should we destroy this first?
            for ( uint32_t i { 0 }; i < m_FrameBuffers.size(); i++ )
            {
                destroyFrameBuffer( core::getLogicalDevice(), m_FrameBuffers[ i ] );
            }
        }
        // Resize the framebuffers
        m_FrameBuffers.resize( m_Swapchain.images.size() );

        for ( uint32_t i { 0 }; i < m_Swapchain.images.size(); i++ )
        {
            destroyFrameBuffer( core::getLogicalDevice(), m_FrameBuffers[ i ] );
        }

        for ( uint32_t i { 0 }; i < m_Swapchain.images.size(); i++ )
        {
            // NOTE(klek): The attachment count here must match the
            //             attachment count in renderpass
            // TODO(klek): Investigate if we can sync these attachments
            uint32_t attachCount = 2;
            utils::vector<VkImageView> attachments { };
            attachments.resize( attachCount );
            attachments[ 0 ] = m_Swapchain.images[ i ].imageView;
            attachments[ 1 ] = m_Swapchain.depthAttachment.view;

            if ( !createFrameBuffer( core::getLogicalDevice(), 
                                     m_Renderpass,
                                     m_Window.getWidth(),
                                     m_Window.getHeight(),
                                     attachCount,
                                     attachments.data(),
                                     m_FrameBuffers[ i ] ) )
            {
                return false;
            }
        }

        return true;
    }

    void vulkan_surface::cleanSwapchain( void )
    {
        destroyImage( core::getLogicalDevice(), &m_Swapchain.depthAttachment );

        // NOTE(klek): Swapchain provides images for us, but not the 
        //             views. Therefor, we are responsible for destroying
        //             only the views...
        //             The swapchain destroys the images for us
        for ( uint32_t i { 0 }; i < m_Swapchain.images.size(); i++ )
        {
            vkDestroyImageView( core::getLogicalDevice(), 
                                m_Swapchain.images[i].imageView, 
                                nullptr );
        }
        vkDestroySwapchainKHR( core::getLogicalDevice(), m_Swapchain.swapchain, nullptr );
    }

    bool getSwapchainDetails( VkPhysicalDevice device,
                              VkSurfaceKHR surface,
                              swapchain_details& details )
    {
        //swapchain_details details { };
        VkResult result { VK_SUCCESS };

        // Capabilities
        // Get the surface capabilities for the given surface on 
        // the given physical device
        result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR( device,
                                                            surface,
                                                            &details.surfaceCapabilities );
        if ( VK_SUCCESS != result )
        {
            MSG("Failed to get surface capabilities...");
            return false;
        }

        // Get the supported formats
        uint32_t formatCount { 0 };
        result = vkGetPhysicalDeviceSurfaceFormatsKHR( device,
                                                       surface,
                                                       &formatCount,
                                                       nullptr );
        if ( VK_SUCCESS != result || formatCount == 0 )
        {
            MSG("Failed to get surface formats...");
            return false;
        }
        details.formats.resize( formatCount );
        result = vkGetPhysicalDeviceSurfaceFormatsKHR( device,
                                                       surface,
                                                       &formatCount,
                                                       details.formats.data() );
        if ( VK_SUCCESS != result )
        {
            MSG("Failed to get surface formats...");
            return false;
        }

        // Get the supported presentation modes
        uint32_t presModeCount { 0 };
        result = vkGetPhysicalDeviceSurfacePresentModesKHR( device,
                                                            surface,
                                                            &presModeCount,
                                                            nullptr );
        if ( VK_SUCCESS != result || presModeCount == 0 )
        {
            MSG("Failed to get surface presentation modes...");
            return false;
        }
        details.presentationModes.resize( presModeCount );
        result = vkGetPhysicalDeviceSurfacePresentModesKHR( device,
                                                            surface,
                                                            &presModeCount,
                                                            details.presentationModes.data() );
        if ( VK_SUCCESS != result )
        {
            MSG("Failed to get surface presentation modes...");
            return false;
        }

        return true;
    }

} // namespace muggy::graphics::vulkan

#endif