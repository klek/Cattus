//********************************************************************
//  File:    vulkanInterface.cpp
//  Date:    Thu, 07 Mar 2024: 16:55
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#include "vulkanCommon.h"
#include "../graphicsPlatformInterface.h"
#include "vulkanInterface.h"
#include "vulkanCore.h"

namespace muggy::graphics::vulkan
{
    namespace // Anonymous namespace, ie only for use in this cpp-file
    {
        
    } // Anonymous namespace, ie only for use in this cpp-file
    
    void getPlatformInterface( platform_interface& platformInterface )
    {
        platformInterface.initialize = core::initialize;
        platformInterface.shutdown = core::shutdown;

        // TODO(klek): Add assignment of the low level surface
        //             function calls here
        platformInterface.surface.create = core::createSurface;
        platformInterface.surface.remove = core::removeSurface;
        platformInterface.surface.resize = core::resizeSurface;
        platformInterface.surface.minimize = core::minimizeSurface;
        platformInterface.surface.getWidth = core::getSurfaceWidth;
        platformInterface.surface.getHeight = core::getSurfaceHeight;
        platformInterface.surface.render = core::renderSurface;

        // Finally specify which platform this interface uses
        platformInterface.platform = graphics_platform::VULKAN;
    }
    
} // namespace muggy::graphics::vulkan
