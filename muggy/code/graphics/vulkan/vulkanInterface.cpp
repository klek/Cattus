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
#include "vulkanCamera.h"

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

        // Assign camera function pointers
        platformInterface.camera.create = camera::create;
        platformInterface.camera.remove = camera::remove;
        platformInterface.camera.setParameter = camera::setParameter;
        platformInterface.camera.getParameter = camera::getParameter;

        // TODO(klek): Add submesh functions here for the low-level
        //             renderer
        //platformInterface.resources.addSubmesh = ?;
        //platformInterface.resources.removeSubmesh = ?;

        // Finally specify which platform this interface uses
        platformInterface.platform = graphics_platform::VULKAN;
    }
    
} // namespace muggy::graphics::vulkan
