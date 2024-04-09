//********************************************************************
//  File:    testRenderer.cpp
//  Date:    Thu, 07 Mar 2024: 16:22
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#include "test.h"
#if TEST_RENDERER
#include "testRenderer.h"

#define NR_OF_SURFACES              4

muggy::graphics::render_surface     surfaces[NR_OF_SURFACES];
test_timer                          timer { };
static uint32_t                     iteration { 0 };
bool isRunning { true };

static bool closeWindow( muggy::event::windowCloseEvent& e )
{
    // If we get here we should close the window
    //muggy::platform::removeWindow( e.getId() );

    uint32_t count = 0;
    // Check if all windows are closed
    for ( int i = 0; i < NR_OF_SURFACES; i++ )
    {
        if ( surfaces[i].window.isClosed() )
        {
            count++;
        }
    }

    if ( count == NR_OF_SURFACES )
    {
        isRunning = false;
    }

    // Return true to indicate we have dealt with the event
    return true;
}

static bool resizeWindow( muggy::event::windowResizeEvent& e )
{
    assert( muggy::id::isValid( e.getId() ) );
    // Get a reference to the surface that got resized
    muggy::graphics::render_surface& surface = surfaces[ e.getId() ];

    // Indicate to the surface that it got resized
    surface.surface.resize( e.getWidth(), e.getHeight() );

    // Return true to indicate we have dealt with the event
    return true;
}

static void onEventCallback( muggy::event::event& e )
{
    muggy::event::eventDispatcher dispatcher(e);

    dispatcher.dispatch<muggy::event::windowCloseEvent>( &closeWindow );
    dispatcher.dispatch<muggy::event::windowResizeEvent>( &resizeWindow );

    //std::cout << e << std::endl;
}

static void createRenderSurface( muggy::graphics::render_surface& surface,
                                 muggy::platform::window_init_info info   )
{
    surface.window = muggy::platform::createWindow( &info );
    surface.surface = muggy::graphics::createSurface( surface.window );
}

static void destroyRenderSurface( muggy::graphics::render_surface& surface )
{
    muggy::graphics::render_surface temp { surface };
    surface = { };
    if ( temp.surface.isValid() )
    {
        muggy::graphics::removeSurface( temp.surface.getId() );
    }
    if ( temp.window.isValid() )
    {
        muggy::platform::removeWindow( temp.window.getId() );
    }
}

bool engineTest::initialize( void )
{
    // Initialize the graphics with specified api
    bool result { muggy::graphics::initialize( muggy::graphics::graphics_platform::VULKAN ) };

    // Check if the graphics could be initialized
    if ( !result )
    {
        return result;
    }

    muggy::platform::window_init_info info [] 
    {
        { &onEventCallback, NULL, "Window Nr 1", 0, 0, 640, 480 },
        { &onEventCallback, NULL, "Window Nr 2", 100, 100, 480, 640 },
        { &onEventCallback, NULL, "Window Nr 3", 200, 200, 300, 200 },
        { &onEventCallback, NULL, "Window Nr 4", 300, 300, 800, 400 }
    };

    for ( uint32_t i = 0; i < NR_OF_SURFACES; i++ )
    {
        createRenderSurface( surfaces[i], info[i] );
    }


    return result;
}

void engineTest::run( void ) 
{
    while ( isRunning ) 
    {
        timer.begin();

        bool allClosed = false;
        for ( uint32_t i = 0; i < NR_OF_SURFACES; i++ )
        {
            if ( !( surfaces[i].window.isClosed() ) )
            {
                surfaces[i].window.update();
                if ( surfaces[i].surface.isValid() )
                {
                    surfaces[i].surface.render();
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        timer.end();
    }
}

void engineTest::shutdown( void ) 
{
    for ( uint32_t i = 0; i < NR_OF_SURFACES; i++ )
    {
        destroyRenderSurface( surfaces[i] );
    }

    // Shutdown the graphics engine
    muggy::graphics::shutdown();
}

#endif