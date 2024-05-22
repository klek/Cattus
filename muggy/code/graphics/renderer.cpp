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

    void surface::minimize( void ) const
    {
        assert( isValid() );
        gfx.surface.minimize( m_Id, true );
    }

    void surface::maximize( void ) const
    {
        assert( isValid() );
        gfx.surface.minimize( m_Id, false );
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

    camera createCamera( camera_init_info info )
    {
        return gfx.camera.create( info );
    }

    void removeCamera( camera_id id )
    {
        assert( id::isValid( id ) );
        gfx.camera.remove( id );
    }

    //****************************************************************
    // Camera class implementation
    // NOTE(klek): The class is declared in engineAPI/cameraAPI.h
    // Set functions
    void camera::setUpDirection( math::fv3d upDir ) const
    {
        assert( isValid() );
        gfx.camera.setParameter( m_Id, camera_parameter::upDirection,
                                 &upDir , sizeof( upDir ) );
    }

    void camera::setFieldOfView( float fov ) const
    {
        assert( isValid() );
        gfx.camera.setParameter( m_Id, camera_parameter::fieldOfView,
                                 &fov , sizeof( fov ) );
    }

    void camera::setAspectRatio( float aspectRatio ) const
    {
        assert( isValid() );
        gfx.camera.setParameter( m_Id, camera_parameter::aspectRatio,
                                 &aspectRatio , sizeof( aspectRatio ) );
    }

    void camera::setViewWidth( float width ) const
    {
        assert( isValid() );
        gfx.camera.setParameter( m_Id, camera_parameter::viewWidth,
                                 &width , sizeof( width ) );
    }

    void camera::setViewHeight( float height ) const
    {
        assert( isValid() );
        gfx.camera.setParameter( m_Id, camera_parameter::viewHeight,
                                 &height , sizeof( height ) );
    }

    void camera::setRange( float nearPlane, float farPlane ) const
    {
        assert( isValid() );
        gfx.camera.setParameter( m_Id, camera_parameter::nearPlane,
                                 &nearPlane , sizeof( nearPlane ) );
        gfx.camera.setParameter( m_Id, camera_parameter::farPlane,
                                 &farPlane , sizeof( farPlane ) );
    }

    // Get functions
    math::fmat4 camera::getView( void ) const
    {
        assert( isValid() );
        math::fmat4 matrix;
        gfx.camera.getParameter( m_Id, camera_parameter::view,
                                 &matrix, sizeof( matrix ) );
        return matrix;
    }

    math::fmat4 camera::getProjection( void ) const
    {
        assert( isValid() );
        math::fmat4 matrix;
        gfx.camera.getParameter( m_Id, camera_parameter::projection,
                                 &matrix, sizeof( matrix ) );
        return matrix;
    }

    math::fmat4 camera::getInverseProjection( void ) const
    {
        assert( isValid() );
        math::fmat4 matrix;
        gfx.camera.getParameter( m_Id, camera_parameter::inverseProjection,
                                 &matrix, sizeof( matrix ) );
        return matrix;
    }

    math::fmat4 camera::getViewProjection( void ) const
    {
        assert( isValid() );
        math::fmat4 matrix;
        gfx.camera.getParameter( m_Id, camera_parameter::viewProjection,
                                 &matrix, sizeof( matrix ) );
        return matrix;
    }

    math::fmat4 camera::getInverseViewProjection( void ) const
    {
        assert( isValid() );
        math::fmat4 matrix;
        gfx.camera.getParameter( m_Id, camera_parameter::inverseViewProjection,
                                 &matrix, sizeof( matrix ) );
        return matrix;
    }

    math::fv3d camera::getUpDirection( void ) const
    {
        assert( isValid() );
        math::fv3d direction;
        gfx.camera.getParameter( m_Id, camera_parameter::upDirection,
                                 &direction, sizeof( direction ) );
        return direction;
    }

    float camera::getNearPlane( void ) const
    {
        assert( isValid() );
        float nearPlane;
        gfx.camera.getParameter( m_Id, camera_parameter::nearPlane,
                                 &nearPlane, sizeof( nearPlane ) );
        return nearPlane;
    }

    float camera::getFarPlane( void ) const
    {
        assert( isValid() );
        float farPlane;
        gfx.camera.getParameter( m_Id, camera_parameter::farPlane,
                                 &farPlane, sizeof( farPlane ) );
        return farPlane;
    }

    float camera::getFieldOfView( void ) const
    {
        assert( isValid() );
        float fov;
        gfx.camera.getParameter( m_Id, camera_parameter::fieldOfView,
                                 &fov, sizeof( fov ) );
        return fov;
    }

    float camera::getAspectRatio( void ) const
    {
        assert( isValid() );
        float aspectRatio;
        gfx.camera.getParameter( m_Id, camera_parameter::aspectRatio,
                                 &aspectRatio, sizeof( aspectRatio ) );
        return aspectRatio;
    }

    float camera::getViewWidth( void ) const
    {
        assert( isValid() );
        float viewWidth;
        gfx.camera.getParameter( m_Id, camera_parameter::viewWidth,
                                 &viewWidth, sizeof( viewWidth ) );
        return viewWidth;
    }

    float camera::getViewHeight( void ) const
    {
        assert( isValid() );
        float viewHeight;
        gfx.camera.getParameter( m_Id, camera_parameter::viewHeight,
                                 &viewHeight, sizeof( viewHeight ) );
        return viewHeight;
    }

    camera::type camera::getProjectionType( void ) const
    {
        assert( isValid() );
        camera::type type;
        gfx.camera.getParameter( m_Id, camera_parameter::type,
                                 &type, sizeof( type ) );
        return type;
    }

    id::id_type camera::getEntityId( void ) const
    {
        assert( isValid() );
        id::id_type entityId;
        gfx.camera.getParameter( m_Id, camera_parameter::entityId,
                                 &entityId, sizeof( entityId ) );
        return entityId;
    }

    //****************************************************************
    id::id_type addSubmesh( const uint8_t *& data )
    {
        return gfx.resources.addSubmesh( data );
    }

    void removeSubmesh( id::id_type id )
    {
        return gfx.resources.removeSubmesh( id );
    }
}