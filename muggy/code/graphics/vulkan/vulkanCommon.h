//********************************************************************
//  File:    vulkanCommon.h
//  Date:    Thu, 07 Mar 2024: 17:05
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(VULKAN_COMMON_H)
#define VULKAN_COMMON_H

#include "../../common/common.h"
#include "../renderer.h"

// TODO(klek): Add vulkan specific includes

#if defined( _WIN64 )
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined( __linux__ )
#define VK_USE_PLATFORM_XLIB_KHR
#define VK_USE_PLATFORM_XCB_KHR
#else
#warning "Unsupported platform"
#endif

#include <vulkan/vulkan.h>
//#include "volk/volk.h"

namespace muggy::graphics::vulkan
{
    namespace constants
    {
        // Constant for desribing maximum "virtual" frames that this vulkan
        // implementation uses
        constexpr uint32_t FRAME_BUFFER_COUNT { 3 };
    } // namespace constants

    struct vulkan_image
    {
        VkImage                     image;
        VkDeviceMemory              memory;
        VkImageView                 view;
        uint32_t                    width;
        uint32_t                    height;
    };

    struct vulkan_renderpass
    {
        enum state : uint32_t
        {
            READY,
            RECORDING,
            IN_RENDER_PASS,
            RECORDING_ENDED,
            SUBMITTED,
            NOT_ALLOCATED
        };

        VkRenderPass                renderPass;
        // Might need to change this
        math::u32v4d                renderArea;
        math::fv4d                  clearColor;
        float                       depth;
        uint32_t                    stencil;
    };

    struct vulkan_cmd_buffer
    {
        enum state : uint32_t
        {
            CMD_READY,
            CMD_RECORDING,
            CMD_IN_RENDER_PASS,
            CMD_RECORDING_ENDED,
            CMD_SUBMITTED,
            CMD_NOT_ALLOCATED
        };

        VkCommandBuffer             cmdBuffer;
        state                       cmdState;
    };

    struct vulkan_framebuffer
    {
        VkFramebuffer               frameBuffer;
        uint32_t                    attachCount;
        utils::vector<VkImageView>  attachments;
        vulkan_renderpass*          renderPass;
    };

    struct vulkan_fence
    {
        VkFence                     fence;
        bool                        signaled;
    };

} // namespace muggy::graphics::vulkan


#if !defined(GLFW)
#define GLFW
#endif
// NOTE(klek): Extension availability
// NOTE(klek): This macro expects the argument to be a vector...
// TODO(klek): Can we do this another way?
#if ( defined(GLFW) || defined(GLFW3) )

//#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#if defined( _WIN64 )
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <windows.h>

#elif defined( __linux__ )
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>
//#include <X11/Xlib.h>
//#include <xcb/xcb.h>

#else
#warning "Unsupported platform"
#endif


inline
void getPlatformExtensions( muggy::utils::vector<const char*> &v )
{                                                        
    if ( glfwVulkanSupported() )
    {
        const char** glfwExt;
        uint32_t glfwExtCount = 0;
        glfwExt = glfwGetRequiredInstanceExtensions( &glfwExtCount );
        for ( uint32_t i = 0; i < glfwExtCount; i++ )
        {
            v.push_back( glfwExt[ i ] );
        }
    }
    else
    {
        MSG("Vulkan not supported according to GLFW...");

        v.push_back( VK_KHR_SURFACE_EXTENSION_NAME );
#if defined( _WIN64 )
        v.push_back( VK_KHR_WIN32_SURFACE_EXTENSION_NAME );
#elif defined( __linux__ )
        // NOTE(klek): These are forced extensions for linux
        v.push_back( VK_KHR_XLIB_SURFACE_EXTENSION_NAME );
        // GLFW uses the xcb surface in addition to the
        // xlib surface
        v.push_back( VK_KHR_XCB_SURFACE_EXTENSION_NAME );
        // v.push_back( VK_KHR_XLIB_SURFACE_EXTENSION_NAME );
        // v.push_back( VK_KHR_XCB_SURFACE_EXTENSION_NAME );
#endif
    }
}

#elif defined( _WIN64 )
// TODO(klek): Verify that this actually works when we are not using 
//             GLFW
inline
void getPlatformExtensions( muggy::utils::vector<const char*> &v )
{
    v.push_back( VK_KHR_SURFACE_EXTENSION_NAME );
    v.push_back( VK_KHR_WIN32_SURFACE_EXTENSION_NAME );
}

#elif defined( __linux__ )
// TODO(klek): Verify that this actually works when we are not using 
//             GLFW
inline
void getPlatformExtensions( muggy::utils::vector<const char*> &v )
{
    v.push_back( VK_KHR_SURFACE_EXTENSION_NAME );
    v.push_back( VK_KHR_XLIB_SURFACE_EXTENSION_NAME );
    // v.push_back( VK_KHR_XCB_SURFACE_EXTENSION_NAME );
}

#else
#error "This platform is not currently supported!"
#endif

#define MUGGY_VULKAN_SETUP_PLATFORM_EXT(v)  getPlatformExtensions( v )

#if !defined( GET_INSTANCE_PROC_ADDR )
#define GET_INSTANCE_PROC_ADDR(inst, entry)                                     \
    {                                                                           \
        fp##entry = (PFN_vk##entry)vkGetInstanceProcAddr( inst, "vk"#entry);    \
        if ( !fp##entry )                                                       \
        {                                                                       \
            MSG("vkGetInstanceProcAddr failed to find vk"#entry);               \
        }                                                                       \
    }
#endif

#if !defined( GET_DEVICE_PROC_ADDR )
#define GET_DEVICE_PROC_ADDR(dev, entry)                                        \
    {                                                                           \
        fp##entry = (PFN_vk##entry)vkGetDeviceProcAddr( dev, "vk"#entry);       \
        if ( !fp##entry )                                                       \
        {                                                                       \
            MSG("vkGetDeviceProcAddr failed to find vk"#entry);                 \
        }                                                                       \
    }
#endif

#endif