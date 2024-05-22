//********************************************************************
//  File:    renderer.h
//  Date:    Mon, 13 Nov 2023: 00:13
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(RENDERER_H)
#define RENDERER_H

#include "../common/common.h"
#include "../platform/window.h"
#include "../../engineAPI/cameraAPI.h"

namespace muggy::graphics
{
    DEFINE_TYPED_ID(surface_id);

    // The high level surface class, which similarly to a window
    // is basically an id
    class surface
    {
    public:
        constexpr explicit surface( surface_id id ) : m_Id( id ) {}
        constexpr surface( ) = default;
        constexpr surface_id getId( ) const { return m_Id; }
        constexpr bool isValid( ) const { return id::isValid( m_Id ); }

        void resize( uint32_t width, uint32_t height ) const;
        void minimize( void ) const;
        void maximize( void ) const;
        uint32_t getWidth( void ) const;
        uint32_t getHeight( void ) const;
        void render( void ) const;

    private:
        surface_id m_Id{ id::invalid_id };
    };

    struct render_surface 
    {
        // Window pointer, for now just empty
        platform::window window{};
        graphics::surface surface{};
    };

    struct camera_parameter
    {
        enum parameter : uint32_t
        {
            upDirection,
            fieldOfView,
            aspectRatio,
            viewWidth,
            viewHeight,
            nearPlane,
            farPlane,
            view,
            projection,
            inverseProjection,
            viewProjection,
            inverseViewProjection,
            type,
            entityId,

            count
        };
    };

    struct camera_init_info
    {
        id::id_type         entityId{ id::invalid_id };
        camera::type        type { };
        math::fv3d          upDirection;
        float               nearPlane;
        float               farPlane;
        // Adding unions here because one of the camera types uses
        // fov and aspectRatio while the other camera type uses
        // viewWidth and viewHeight
        union
        {
            float fov;
            float viewWidth;
        };
        union
        {
            float aspectRatio;
            float viewHeight;
        };
    };

    // Helper structure to create a perspective camera init info
    struct perspective_camera_init_info : public camera_init_info
    {
        explicit perspective_camera_init_info( id::id_type id )
        {
            assert( id::isValid( id ) );
            entityId = id;
            type = camera::perspective;
            // NOTE(klek): Considering a right-handed coordinate
            //             system, the positive y-axis is here by
            //             default representing the up-direction
            upDirection = { 0.0f, 1.0f, 0.0f };
            fov = 0.25f;
            aspectRatio = 16.0f / 10.0f;
            nearPlane = 0.001f;
            farPlane = 10000.0f;
        }
    };

    // Helper structure to create an orthographic camera init info
    struct orthographic_camera_init_info : public camera_init_info
    {
        explicit orthographic_camera_init_info( id::id_type id )
        {
            assert( id::isValid( id ) );
            entityId = id;
            type = camera::orthographic;
            // NOTE(klek): Considering a right-handed coordinate
            //             system, the positive y-axis is here by
            //             default representing the up-direction
            upDirection = { 0.0f, 1.0f, 0.0f };
            viewWidth = 1920;
            viewHeight = 1080;
            nearPlane = 0.001f;
            farPlane = 10000.0f;
        }
    };

    enum class graphics_platform : uint32_t
    {
        OPEN_GL     = 0,
        DIRECT3D12  = 1,
        VULKAN      = 2, // etc etc
    };

    bool initialize( graphics_platform platform );
    void shutdown( void );

    // Get the location of compiled engine shaders relative to the
    // executables path. The path is for the graphics API currently
    // in use.
    const char* getEngineShadersPath( void );
    // Get the location of compiled engine shaders, for the specified
    // platform, relative to the executables path.
    const char* getEngineShadersPath( graphics_platform platform );

    // High-level create/remove a surface
    surface createSurface( platform::window window );
    void removeSurface( surface_id id );

    // High-level create/remove camera
    camera createCamera( camera_init_info info );
    void removeCamera( camera_id id );
}

#endif