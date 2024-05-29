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
#include "../../muggy/code/event/event.h"
#include "../../muggy/code/platform/platform.h"
#include "../../muggy/code/platform/window.h"
#include "../../muggy/code/platform/platformTypes.h"
#include "../../muggy/code/graphics/renderer.h"
#include "../../muggy/code/components/entity.h"
#include "../../muggy/code/components/transform.h"

#define NR_OF_SURFACES              4

// Variable for holding a game entity
muggy::game_entity::entity          entity { };
// Variable for holding the camera
muggy::graphics::camera             camera { };

muggy::graphics::render_surface     surfaces[NR_OF_SURFACES];
test_timer                          timer { };
static uint32_t                     iteration { 0 };
bool isRunning { true };

bool initRenderSurfaces( void );
void shutdownRenderSurfaces( void );

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

    if ( e.getWidth() == 0 || e.getHeight() == 0 )
    {
        surface.surface.minimize( );
    }
    else
    {
        surface.surface.maximize( );
        // Indicate to the surface that it got resized
        surface.surface.resize( e.getWidth(), e.getHeight() );
    }

    // Return true to indicate we have dealt with the event
    return true;
}

static bool keyWindow( muggy::event::keyReleasedEvent& e )
{
    assert( muggy::id::isValid( e.getId() ) );

    bool result { false };

    // Get a reference to the surface that got the key
    muggy::graphics::render_surface& surface { surfaces[ e.getId() ] };

    uint32_t key { e.getKeyCode() };
    // Check if our desired key was released
    switch ( key )
    {
        // Nr 82 should correspond to r-key
        case 82:
        {
            // We want to reload the graphics api here
            shutdownRenderSurfaces( );
            result = initRenderSurfaces( );
            assert( result );
            break;
        }
        default:
            break;
    }

    // Return true to indicate we have dealt with the event
    return true;
}

static void onEventCallback( muggy::event::event& e )
{
    muggy::event::eventDispatcher dispatcher(e);

    dispatcher.dispatch<muggy::event::windowCloseEvent>( &closeWindow );
    dispatcher.dispatch<muggy::event::windowResizeEvent>( &resizeWindow );
    dispatcher.dispatch<muggy::event::keyReleasedEvent>( &keyWindow );

    //std::cout << e << std::endl;
}

muggy::game_entity::entity createOneGameEntity( void )
{
    // Setup the transform component
    muggy::transform::init_info transformInfo { };
    muggy::math::fv3d rot { 0.0f, 0.0f, 3.14f };
    muggy::math::fv4d quat { muggy::math::toQuaternionFromEuler( rot ).vec };
    // Copy data into our transform info
    memcpy( &transformInfo.rotation[0], &quat.x, sizeof( transformInfo.rotation ) );

    // Create the game entity
    muggy::game_entity::entity_info entityInfo { };
    entityInfo.transform = &transformInfo;
    muggy::game_entity::entity ntt { muggy::game_entity::createGameEntity( entityInfo ) };
    assert( ntt.isValid() );

    return ntt;
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

bool initRenderSurfaces( void )
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
        { &onEventCallback, NULL, "Window Nr 1", 100, 100, 640, 480 },
        { &onEventCallback, NULL, "Window Nr 2", 400, 400, 480, 640 },
        { &onEventCallback, NULL, "Window Nr 3", 200, 200, 300, 200 },
        { &onEventCallback, NULL, "Window Nr 4", 300, 300, 800, 400 }
    };

    for ( uint32_t i = 0; i < NR_OF_SURFACES; i++ )
    {
        createRenderSurface( surfaces[i], info[i] );
    }

    // TODO(klek): Load test model here later

    // Create a singular game entity
    entity = createOneGameEntity( );
    // Create a camera that is using our game entity
    camera = muggy::graphics::createCamera( muggy::graphics::perspective_camera_init_info( entity.getId() ) );
    assert( camera.isValid() );

    return result;
}

void shutdownRenderSurfaces( void )
{
    // Remove the camera
    if ( camera.isValid() )
    {
        muggy::graphics::removeCamera( camera.getId() );
    }

    // Remove the game entity
    if ( entity.isValid() )
    {
        muggy::game_entity::removeGameEntity( entity.getId() );
    }

    for ( uint32_t i = 0; i < NR_OF_SURFACES; i++ )
    {
        destroyRenderSurface( surfaces[i] );
    }

    // Shutdown the graphics engine
    muggy::graphics::shutdown();
}

bool engineTest::initialize( void )
{
    return initRenderSurfaces( );
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
    shutdownRenderSurfaces( );
}

#endif