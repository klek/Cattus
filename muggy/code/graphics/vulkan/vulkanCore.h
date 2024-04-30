//********************************************************************
//  File:    vulkanCore.h
//  Date:    Thu, 07 Mar 2024: 16:58
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(VULKAN_CORE_H)
#define VULKAN_CORE_H

#include "vulkanCommon.h"

namespace muggy::graphics::vulkan::core
{
    bool initialize( void );
    void shutdown( void );

    bool createDevice( VkSurfaceKHR surface );
    bool createGraphicsCommand( uint32_t swapchainBufferSize );
    bool detectDepthFormat( VkPhysicalDevice physicalDevice );
    bool findMemoryIndex( uint32_t type, uint32_t flags, uint32_t& index );

    // Helper functions
    uint32_t getGraphicsFamilyQueueIndex( void );
    uint32_t getPresentationFamilyQueueIndex( void );
    uint32_t getCopyFamilyQueueIndex( void );
    uint32_t getComputeFamilyQueueIndex( void );
    bool getPresentSupported( const uint32_t index );
    VkFormat getDepthFormat( void );
    VkPhysicalDevice getPhysicalDevice( void );
    VkDevice getLogicalDevice( void );
    VkInstance getInstance( void );
    uint32_t getCurrentFrameIndex( void );

    // Low-level functions for vulkan surfaces
    surface createSurface( platform::window window );
    void removeSurface( surface_id id );
    void resizeSurface( surface_id id, uint32_t width, uint32_t height );
    void minimizeSurface( surface_id id, bool isMinimized );
    uint32_t getSurfaceWidth( surface_id id );
    uint32_t getSurfaceHeight( surface_id id );
    void renderSurface( surface_id id /*, frame_info info*/ );

} // namespace muggy::graphics::vulkan::core


#endif