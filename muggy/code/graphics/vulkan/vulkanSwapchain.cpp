//********************************************************************
//  File:    vulkanSwapchain.cpp
//  Date:    Sun, 21 Apr 2024: 19:37
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#include "vulkanSwapchain.h"
#include "vulkanHelpers.h"
#include "vulkanCore.h"
#include "vulkanResources.h"
#include "vulkanRenderPass.h"
#include "vulkanContent.h"

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
            MSG("VK_IMAGE_USAGE_TRANSFER_DST image usage is not supported by the swap chain...");
            // Transfer destination not available
            return VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM;
        }
    } // namespace anonymous

    //****************************************************************
    // vulkan_swapchain implementation
    // TODO(klek): Split this function into smaller ones such that the
    //             overview gets better!
    bool vulkan_swapchain::createSwapchain( const VkSurfaceKHR& surface )
    {
        // We pick the best settings for the swapchain based on the
        // swapchain details from the physical device
        bool res = getSwapchainDetails( core::getPhysicalDevice(), surface, m_SwapchainDetails );
        assert( res );

        // Find optimal surface values for the swapchain
        VkSurfaceFormatKHR format { chooseBestSurfaceFormat( m_SwapchainDetails.formats ) };
        VkPresentModeKHR mode { chooseBestPresentationMode( m_SwapchainDetails.presentationModes ) };
        VkExtent2D extent { chooseBestSwapExtent( m_SwapchainDetails.surfaceCapabilities, 
                                                  m_Extent.width, 
                                                  m_Extent.height ) };
        VkImageUsageFlags usageFlags { getImageUsageFlags( m_SwapchainDetails.surfaceCapabilities ) };

        // Check that the desired image usage flags is valid
        if ( VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM == usageFlags )
        {
            return false;
        }

        uint32_t imagesInFlight = m_SwapchainDetails.surfaceCapabilities.minImageCount + 1;
        // NOTE(klek): At this point, in a typical situation, imagesInFlight 
        //             will be at least 2, allowing us to use triple buffer.
        //             However, there will be a rare occasion that triple
        //             buffering is not supported. If that is the case, we
        //             will need to set imagesInFlight to the max image
        //             count supported. If maxImageCount == 0, there is no
        //             max limit
        if ( ( m_SwapchainDetails.surfaceCapabilities.maxImageCount > 0 ) &&
             ( m_SwapchainDetails.surfaceCapabilities.maxImageCount < imagesInFlight ) )
        {
            imagesInFlight = m_SwapchainDetails.surfaceCapabilities.maxImageCount;
        }

        // Create the swapchain info
        VkSwapchainCreateInfoKHR info { };
        info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        info.pNext = nullptr;
        info.flags = 0;
        info.surface = surface;
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
                static_cast<uint32_t>( core::getGraphicsFamilyQueueIndex() ),
                static_cast<uint32_t>(core::getPresentationFamilyQueueIndex() )
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
        info.preTransform = m_SwapchainDetails.surfaceCapabilities.currentTransform;
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
        info.oldSwapchain = ( ( m_OldSwapchain == VK_NULL_HANDLE ) ? 
                                VK_NULL_HANDLE : m_OldSwapchain->m_Swapchain );

        // Chech to make sure that the given surface is supported by
        // the device for each queue
        // TODO(klek): Extend this when using compute and transfer
        //             queues
        VkBool32 supported { false };
        VkResult result { VK_SUCCESS };
        result = vkGetPhysicalDeviceSurfaceSupportKHR( core::getPhysicalDevice(),
                                                       core::getGraphicsFamilyQueueIndex(),
                                                       surface,
                                                       &supported );
        if ( ( VK_SUCCESS != result ) || ( VK_FALSE == supported ) )
        {
            MSG("Physical device does not support graphics family queue for this surface...");
            return false;
        }
        result = vkGetPhysicalDeviceSurfaceSupportKHR( core::getPhysicalDevice(),
                                                       core::getPresentationFamilyQueueIndex(),
                                                       surface,
                                                       &supported );
        if ( ( VK_SUCCESS != result ) || ( VK_FALSE == supported ) )
        {
            MSG("Physical device does not support presentation family queue for this surface...");
            return false;
        }

        result = vkCreateSwapchainKHR( core::getLogicalDevice(), 
                                       &info, 
                                       nullptr, 
                                       &m_Swapchain );
        if ( VK_SUCCESS != result )
        {
            MSG("Failed to create swapchain...");
            return false;
        }

        m_ImageFormat = format.format;
        m_Extent = extent;

        // Get swapchain images and image views
        //uint32_t imageCount { 0 };
        result = vkGetSwapchainImagesKHR( core::getLogicalDevice(),
                                          m_Swapchain,
                                          &m_ImageCount,
                                          nullptr );
        if ( ( VK_SUCCESS != result ) || ( m_ImageCount == 0 ) )
        {
            MSG("Failed to get the number of swap chain images...");
            return false;
        }
        utils::vector<VkImage> images( m_ImageCount );
        result = vkGetSwapchainImagesKHR( core::getLogicalDevice(),
                                          m_Swapchain,
                                          &m_ImageCount,
                                          images.data() );
        if ( VK_SUCCESS != result )
        {
            MSG("Failed to get swap chain images...");
            return false;
        }

        // For every image we found in the swapchain we just created,
        // we generate an image view and push them both into our 
        // member-variable
        for ( const auto& image : images )
        {
            swapchain_image temp { };
            temp.image = image;
            VkImageViewCreateInfo info { 
                vulkan_utils::setImageViewInfo( image, m_ImageFormat, VK_IMAGE_ASPECT_COLOR_BIT )
            };
            info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            result = vkCreateImageView( core::getLogicalDevice(), &info, nullptr, &temp.imageView );
            if ( VK_SUCCESS != result )
            {
                MSG("Failed to create image view...")
                // TODO(klek): Handle error here!!
            }
            m_SwapChainImages.push_back( temp );
        }

        // Look for a depthformat
        if ( !core::detectDepthFormat( core::getPhysicalDevice() ) )
        {
            MSG("Failed to find a supported depth format...");
            return false;
        }

        // Create depthbuffer image and view
        image_init_info imageInfo { };
        imageInfo.device = core::getLogicalDevice();
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.width = m_Extent.width;
        imageInfo.height = m_Extent.height;
        imageInfo.format = core::getDepthFormat();
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        imageInfo.memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        imageInfo.createView = true;
        imageInfo.viewAspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;

        if ( !createImage( &imageInfo, m_DepthAttachment ) )
        {
            return false;
        }

        MSG("Swapchain created successfully!");

        return true;
    }

    void vulkan_swapchain::release( void )
    {
        // Should we wait for the device to become idle here?
        // vkDeviceWaitIdle( core::getLogicalDevice() );

        // Destroy the depth attachment
        destroyImage( core::getLogicalDevice(), &m_DepthAttachment );
        
        // Destroy all image views
        // NOTE(klek): Swapchain provides images for us, but not the 
        //             views. Therefor, we are responsible for destroying
        //             only the views...
        //             The swapchain destroys the images for us
        for ( uint32_t i { 0 }; i < m_SwapChainImages.size(); i++ )
        {
            vkDestroyImageView( core::getLogicalDevice(), 
                                m_SwapChainImages[i].imageView, 
                                nullptr );
        }

        // Destroy the swapchain
        if ( VK_NULL_HANDLE != m_Swapchain )
        {
            vkDestroySwapchainKHR( core::getLogicalDevice(), m_Swapchain, nullptr );
            m_Swapchain = VK_NULL_HANDLE;
        }

        // Finally clear the vector
        m_SwapChainImages.clear();
    }

    VkResult vulkan_swapchain::getNextImageIndex( const VkSemaphore& imageAvailable, 
                                                  const VkFence& fence, uint32_t& currentIndex )
    {
        VkResult result { VK_SUCCESS };
        result = vkAcquireNextImageKHR( core::getLogicalDevice(),
                                        m_Swapchain, uint64_invalid_id,
                                        imageAvailable, fence, &m_CurrentImageIndex );
        if ( VK_SUCCESS == result )
        {
            currentIndex = m_CurrentImageIndex;
        }
        return result;
    }

    VkImage& vulkan_swapchain::getCurrentImage( void )
    {
        return m_SwapChainImages[ m_CurrentImageIndex ].image;
    }

    VkResult vulkan_swapchain::present( const VkQueue& queue, 
                                        const VkSemaphore& renderFinished )
    {
        VkPresentInfoKHR info { vulkan_utils::setPresentInfo( &m_Swapchain, &renderFinished, &m_CurrentImageIndex ) };
        return vkQueuePresentKHR( queue, &info );
    }

    //****************************************************************
    // vulkan_surface implementation
    bool vulkan_surface::create( VkInstance instance )
    {
        // Creating a vulkan surface happens before creating a device
        // because a vulkan device needs information from the surface
        // creation
        bool result { true };
        // TODO(klek): Add proper error handling
        result &= createSurface( instance );
        assert( result );
        // TODO(klek): Add proper error handling
        result &= core::createDevice( m_Surface );
        assert( result );
        // TODO(klek): Add proper error handling
        result &= reCreateSwapchain( );
        assert( result );
        // TODO(klek): Add potentially renderpass etc here
        result &= createRenderpass( );
        assert( result );
        // TODO(klek): handle framebuffers
        result &= reCreateFrameBuffers( );
        assert( result );

        // TODO(klek): Add proper error handling
        result &= core::createGraphicsCommand( 0 );
        assert( result );

        // TODO(klek): Add proper error handling
        result &= content::init( m_Renderpass.renderPass );
        assert( result );

        return result;
    }

    void vulkan_surface::release( void )
    {
        vkDeviceWaitIdle( core::getLogicalDevice() );

        // Destroy framebuffer
        // TODO(klek): Implement a loop to destroy all framebuffers
        if ( m_FrameBuffers.size() > 0 )
        {
            // This means that we have old data...
            // Should we destroy this first?
            for ( vulkan_framebuffer& buffer : m_FrameBuffers )
            {
                destroyFrameBuffer( core::getLogicalDevice(), buffer );
            }
        }
        // Destroy the renderpass
        if ( VK_NULL_HANDLE != m_Renderpass.renderPass )
        {
            renderpass::destroyRenderPass( core::getLogicalDevice(), m_Renderpass );
        }
        // Move the swapchain into temporary, such that it can
        // be destroyed before the surface
        if ( VK_NULL_HANDLE != m_Swapchain )
        {
            std::shared_ptr<vulkan_swapchain> temp { std::move( m_Swapchain ) };
            m_Swapchain = VK_NULL_HANDLE;
        }
        // Destroy the surface
        if ( VK_NULL_HANDLE != m_Surface )
        {
            vkDestroySurfaceKHR( core::getInstance(), m_Surface, nullptr );
            m_Surface = VK_NULL_HANDLE;
        }
    }

    void vulkan_surface::setRenderArea( math::u32v4d renderArea )
    {
        m_Renderpass.renderArea = renderArea;
    }

    void vulkan_surface::setClearColor( math::fv4d clearColor )
    {
        m_Renderpass.clearColor = clearColor;
    }

    bool vulkan_surface::getNextImage( const VkSemaphore& imageAvailable,
                                       const VkFence& fence )
    {
        assert( !m_IsFrameStarted );
        // Ask the swapchain for the next image index
        VkResult result { VK_SUCCESS };
        result = m_Swapchain.get()->getNextImageIndex( imageAvailable, fence, m_CurrentImageIndex );
        if ( VK_ERROR_OUT_OF_DATE_KHR == result )
        {
            reCreateSwapchain( );
            return false;
        }
        else if ( VK_SUCCESS != result && VK_SUBOPTIMAL_KHR != result )
        {
            MSG("Failed to acquire next swapchain image...");
            return false;
        }
        // Set internal variable for frame started
        m_IsFrameStarted = true;
        return true;
    }

    VkImage& vulkan_surface::getCurrentImage( void )
    {
        return m_Swapchain.get()->getCurrentImage();
    }

    bool vulkan_surface::present( const VkQueue& queue,
                                  const VkSemaphore& renderFinished )
    {
        assert( m_IsFrameStarted );
        // Tell the swapchain to present
        VkResult result { VK_SUCCESS };
        result = m_Swapchain.get()->present( queue, renderFinished );
        if ( VK_ERROR_OUT_OF_DATE_KHR == result ||
             VK_SUBOPTIMAL_KHR == result ||
             m_IsResized )
        {
            m_IsResized = false;
            reCreateSwapchain( );
            // Resetting the frame has started, because we just return
            // from this function
            m_IsFrameStarted = false;
            return false;
        }
        else if ( VK_SUCCESS != result )
        {
            MSG("Failed to present...");
            return false;
        }
        // Reset internal variable for frame started
        m_IsFrameStarted = false;
        return true;
    }

    void vulkan_surface::beginRenderPass( const VkCommandBuffer& cmdBuffer )
    {
        renderpass::beginRenderPass( cmdBuffer,
                                     m_Renderpass,
                                     m_FrameBuffers[ m_CurrentImageIndex ].frameBuffer );
    }

    void vulkan_surface::endRenderPass( const VkCommandBuffer& cmdBuffer )
    {
        renderpass::endRenderPass( cmdBuffer );
    }

    bool vulkan_surface::reCreateSwapchain( void )
    {
        // For debugging 
        vkDeviceWaitIdle( core::getLogicalDevice() );

        // Grab the current extent from the window
        m_Extent = { m_Window.getWidth(), m_Window.getHeight() };

        // Is the current swapchain null?
        if ( VK_NULL_HANDLE == m_Swapchain )
        {
            m_Swapchain = std::make_unique<vulkan_swapchain>( m_Surface, m_Extent );
        }
        else
        {
            // Store the old swapchain that we are replacing in a new
            // local shared pointer, that will be destroyed when we
            // leave this scope
            std::shared_ptr<vulkan_swapchain> oldSwapchain { std::move( m_Swapchain ) };
            // Use the old swapchain to create the new
            m_Swapchain = std::make_unique<vulkan_swapchain>( m_Surface, 
                                                              m_Extent, 
                                                              oldSwapchain );
            // Check that the new and old formats are the same
            if ( !oldSwapchain->compareSwapchainFormats( *m_Swapchain.get() ) )
            {
                MSG("Swapchain formats have change...");
                return false;
            }
        }
        // Check that swapchain is actually not null
        if ( VK_NULL_HANDLE == m_Swapchain.get()->getSwapchain() )
        {
            MSG("Failed to get a swapchain...");
            return false;
        }
        // If we have a valid renderpass, we also create framebuffers
        if ( VK_NULL_HANDLE != m_Renderpass.renderPass )
        {
            if ( !reCreateFrameBuffers( ) )
            {
                MSG("Failed to recreate framebuffers...");
                return false;
            }
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
                                              m_Swapchain.get()->getSwapchainImageFormat(),
                                              core::getDepthFormat(),
                                              { 0, 0, m_Window.getWidth(), m_Window.getHeight() },
                                              { 0.0f, 0.0f, 0.0f, 0.0f },
                                              1.0f,
                                              0,
                                              m_Renderpass );
    }

    // NOTE(klek): 
    bool vulkan_surface::reCreateFrameBuffers( void )
    {
        if ( m_FrameBuffers.size() > 0 )
        {
            // This means that we have old data...
            // Should we destroy this first?
            for ( vulkan_framebuffer& buffer : m_FrameBuffers )
            {
                destroyFrameBuffer( core::getLogicalDevice(), buffer );
            }
        }
        // Resize to swapchain image count
        m_FrameBuffers.resize( m_Swapchain.get()->getImageCount() );

        for ( uint32_t i { 0 }; i < m_FrameBuffers.size(); i++ )
        {
            // Get a reference to the current framebuffer
            vulkan_framebuffer& frameBuffer { m_FrameBuffers[ i ] };

            uint32_t attachCount { 2 };
            utils::vector<VkImageView> attachments ( attachCount );
            attachments[ 0 ] = m_Swapchain.get()->getSwapchainImage( i ).imageView;
            attachments[ 1 ] = m_Swapchain.get()->getSwapchainDepthAttachment().view;

            if ( !createFrameBuffer( core::getLogicalDevice(),
                                    m_Renderpass,
                                    m_Extent.width,
                                    m_Extent.height,
                                    attachCount,
                                    attachments.data(),
                                    frameBuffer ) )
            {
                return false;
            }
        }
        return true;
    }

    bool getSwapchainDetails( VkPhysicalDevice device,
                              VkSurfaceKHR surface,
                              swapchain_details& details )
    {
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
