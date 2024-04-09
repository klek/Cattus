//********************************************************************
//  File:    renderer.cpp
//  Date:    Mon, 13 Nov 2023: 00:21
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#include "renderer.h"
#include "graphicsPlatformInterface.h"
#include "opengl/openglInterface.h"
#include "vulkan/vulkanInterface.h"
#include "direct3d12/d3d12Interface.h"

namespace muggy::graphics 
{
    namespace // Anonymous namespace, ie only for use in this cpp-file
    {
        // Temporary solution for paths to compiled engine-shader blobs
        constexpr const char* engineShaderPaths[]{
            "../resources/opengl/shadersBlob.bin",
            "../resources/d3d12/shadersBlob.bin",
            "../resources/vulkan/shadersBlob.bin",
        };

        platform_interface gfx{};

        bool setPlatformInterface( graphics_platform platform )
        {
            switch ( platform )
            {
                case graphics_platform::OPEN_GL :
                {
                    // Call OpenGL specific platform function
                    opengl::getPlatformInterface( gfx );
                    // OpenGL support is currently not implemented
                    return false;
                    break;
                }
                case graphics_platform::DIRECT3D12 :
                {
                    // Call OpenGL specific platform function
                    // NOTE(klek): D3D12 should only be supported on
                    //             windows platforms
                    #if defined( _WIN64 )
                    d3d12::getPlatformInterface( gfx );
                    return false;
                    #else
                    return false;
                    #endif
                    // Direct3D12 support is currently not implemented
                    return false;
                    break;
                }

                case graphics_platform::VULKAN :
                {
                    // Call Vulkan specific platform function
                    // NOTE(klek): Vulkan should be supported on all 
                    //             platforms
                    vulkan::getPlatformInterface( gfx );
                    // Vulkan support is currently not implemented
                    //return false;
                    break;
                }
                
                default:
                {
                    return false;
                    //break;
                }
            }

            // Check that the requested api was set up
            assert( gfx.platform == platform );

            // If we reach here we have an interface?
            return true;
        }

    } // Anonymous namespace, ie only for use in this cpp-file

    bool initialize( graphics_platform platform )
    {
        if ( setPlatformInterface( platform ) )
        {
            return gfx.initialize();
        }
        
        return false;
        //return setPlatformInterface( platform ) && gfx.initialize();
    }

    void shutdown( void )
    {
        if ( gfx.platform != ( (graphics_platform)-1 ) ) 
        {
            gfx.shutdown();
        }
    }

    const char* getEngineShadersPath( void )
    {
        return engineShaderPaths[ (uint32_t)gfx.platform ];
    }

    const char* getEngineShadersPath( graphics_platform platform )
    {
        return engineShaderPaths[ (uint32_t)platform ];
    }

    void surface::resize( uint32_t width, uint32_t height ) const
    {
        assert( isValid() );
        gfx.surface.resize( m_Id, width, height );
    }

    uint32_t surface::getWidth( void ) const
    {
        assert( isValid() );
        return gfx.surface.getWidth( m_Id );
    }
    
    uint32_t surface::getHeight( void ) const
    {
        assert( isValid() );
        return gfx.surface.getHeight( m_Id );
    }
    
    void surface::render( void ) const
    {
        assert( isValid() );
        gfx.surface.render( m_Id );
    }

    surface createSurface( platform::window window )
    {
        return gfx.surface.create( window );
    }

    void removeSurface( surface_id id )
    {
        assert( id::isValid( id ) );
        gfx.surface.remove( id );
    }

}