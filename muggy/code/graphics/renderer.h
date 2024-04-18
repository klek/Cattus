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

}

#endif