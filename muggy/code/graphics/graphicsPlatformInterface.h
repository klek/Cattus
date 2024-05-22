//********************************************************************
//  File:    graphicsPlatformInterface.h
//  Date:    Mon, 13 Nov 2023: 00:56
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(GRAPHICS_PLATFORM_INTERFACE_H)
#define GRAPHICS_PLATFORM_INTERFACE_H

#include "../common/common.h"
#include "renderer.h"
#include "../platform/window.h"

namespace muggy::graphics
{
    struct platform_interface
    {
        bool(*initialize)( void );
        void(*shutdown)( void );

        struct {
            surface(*create)( platform::window );
            void(*remove)( surface_id );
            void(*resize)( surface_id, uint32_t, uint32_t );
            void(*minimize)( surface_id, bool );
            uint32_t(*getWidth)( surface_id );
            uint32_t(*getHeight)( surface_id );
            void(*render)( surface_id );
        } surface;

        struct {
            camera(*create)( camera_init_info );
            void(*remove)( camera_id );
            void(*setParameter)( camera_id, camera_parameter::parameter,
                                 const void * const, uint32_t );
            void(*getParameter)( camera_id, camera_parameter::parameter,
                                 void * const, uint32_t );
        } camera;

        struct {
            id::id_type(*addSubmesh)( const uint8_t*& );
            void(*removeSubmesh)( id::id_type );
        } resources;

        // Initialize platform to a non-valid value
        graphics_platform platform = (graphics_platform)-1;
    };
}


#endif