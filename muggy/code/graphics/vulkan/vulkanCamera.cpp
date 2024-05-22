//********************************************************************
//  File:    vulkanCamera.cpp
//  Date:    Tue, 07 May 2024: 21:33
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#include "vulkanCamera.h"
#include "../../../engineAPI/gameEntity.h"

namespace muggy::graphics::vulkan::camera
{
    namespace
    {
        // Set functions
        void setUpDirection( vulkan_camera camera, const void * const data, [[maybe_unused]] uint32_t size )
        {
            math::fv3d upDirection { *(math::fv3d*)data };
            assert( sizeof( upDirection ) == size );
            camera.setUpDirection( upDirection );
        }

        void setFieldOfView( vulkan_camera camera, const void * const data, [[maybe_unused]] uint32_t size )
        {
            assert( camera.getProjectionType() == graphics::camera::perspective );
            float fov { *(float*)data };
            assert( sizeof( fov ) == size );
            camera.setFieldOfView( fov );
        }

        void setAspectRatio( vulkan_camera camera, const void * const data, [[maybe_unused]] uint32_t size )
        {
            assert( camera.getProjectionType() == graphics::camera::perspective );
            float aspectRatio { *(float*)data };
            assert( sizeof( aspectRatio ) == size );
            camera.setAspectRatio( aspectRatio );
        }

        void setViewWidth( vulkan_camera camera, const void * const data, [[maybe_unused]] uint32_t size )
        {
            assert( camera.getProjectionType() == graphics::camera::orthographic );
            float width { *(float*)data };
            assert( sizeof( width ) == size );
            camera.setViewWidth( width );
        }

        void setViewHeight( vulkan_camera camera, const void * const data, [[maybe_unused]] uint32_t size )
        {
            assert( camera.getProjectionType() == graphics::camera::orthographic );
            float height { *(float*)data };
            assert( sizeof( height ) == size );
            camera.setViewHeight( height );
        }

        void setNearPlane( vulkan_camera camera, const void * const data, [[maybe_unused]] uint32_t size )
        {
            float nearPlane { *(float*)data };
            assert( sizeof( nearPlane ) == size );
            camera.setNearPlane( nearPlane );
        }

        void setFarPlane( vulkan_camera camera, const void * const data, [[maybe_unused]] uint32_t size )
        {
            float farPlane { *(float*)data };
            assert( sizeof( farPlane ) == size );
            camera.setFarPlane( farPlane );
        }

        // Get functions
        void getUpDirection( vulkan_camera camera, void * const data, [[maybe_unused]] uint32_t size )
        {
            math::fv3d *const vector { (math::fv3d *const)data };
            assert( sizeof( math::fv3d ) == size );
            *vector = camera.getUpDirection();
        }

        void getFieldOfView( vulkan_camera camera, void * const data, [[maybe_unused]] uint32_t size )
        {
            assert( camera.getProjectionType() == graphics::camera::perspective );
            float *const fov { (float *const)data };
            assert( sizeof( float ) == size );
            *fov = camera.getFieldOfView();
        }

        void getAspectRatio( vulkan_camera camera, void * const data, [[maybe_unused]] uint32_t size )
        {
            assert( camera.getProjectionType() == graphics::camera::perspective );
            float *const aspectRatio { (float *const)data };
            assert( sizeof( float ) == size );
            *aspectRatio = camera.getAspectRatio();
        }

        void getViewWidth( vulkan_camera camera, void * const data, [[maybe_unused]] uint32_t size )
        {
            assert( camera.getProjectionType() == graphics::camera::orthographic );
            float *const width { (float *const)data };
            assert( sizeof( float ) == size );
            *width = camera.getViewWidth();
        }

        void getViewHeight( vulkan_camera camera, void * const data, [[maybe_unused]] uint32_t size )
        {
            assert( camera.getProjectionType() == graphics::camera::orthographic );
            float *const height { (float *const)data };
            assert( sizeof( float ) == size );
            *height = camera.getViewHeight();
        }

        void getNearPlane( vulkan_camera camera, void * const data, [[maybe_unused]] uint32_t size )
        {
            float *const nearPlane { (float *const)data };
            assert( sizeof( float ) == size );
            *nearPlane = camera.getNearPlane();
        }

        void getFarPlane( vulkan_camera camera, void * const data, [[maybe_unused]] uint32_t size )
        {
            float *const farPlane { (float *const)data };
            assert( sizeof( float ) == size );
            *farPlane = camera.getFarPlane();
        }

        void getView( vulkan_camera camera, void * const data, [[maybe_unused]] uint32_t size )
        {
            math::fmat4 *const matrix { (math::fmat4 *const)data };
            assert( sizeof( math::fmat4 ) == size );
            *matrix = camera.getView();
        }

        void getProjection( vulkan_camera camera, void * const data, [[maybe_unused]] uint32_t size )
        {
            math::fmat4 *const matrix { (math::fmat4 *const)data };
            assert( sizeof( math::fmat4 ) == size );
            *matrix = camera.getProjection();
        }

        void getInverseProjection( vulkan_camera camera, void * const data, [[maybe_unused]] uint32_t size )
        {
            math::fmat4 *const matrix { (math::fmat4 *const)data };
            assert( sizeof( math::fmat4 ) == size );
            *matrix = camera.getInverseProjection();
        }

        void getViewProjection( vulkan_camera camera, void * const data, [[maybe_unused]] uint32_t size )
        {
            math::fmat4 *const matrix { (math::fmat4 *const)data };
            assert( sizeof( math::fmat4 ) == size );
            *matrix = camera.getViewProjection();
        }

        void getInverseViewProjection( vulkan_camera camera, void * const data, [[maybe_unused]] uint32_t size )
        {
            math::fmat4 *const matrix { (math::fmat4 *const)data };
            assert( sizeof( math::fmat4 ) == size );
            *matrix = camera.getInverseViewProjection();
        }

        void getProjectionType( vulkan_camera camera, void * const data, [[maybe_unused]] uint32_t size )
        {
            graphics::camera::type *const type { (graphics::camera::type *const)data };
            assert( sizeof( graphics::camera::type ) == size );
            *type = camera.getProjectionType();
        }

        void getEntityId( vulkan_camera camera, void * const data, [[maybe_unused]] uint32_t size )
        {
            id::id_type *const entityId { (id::id_type *const)data };
            assert( sizeof( id::id_type ) == size );
            *entityId = camera.getEntityId();
        }

        // Dummy function to be used for filling up the remaining
        // unused slots in the set-funtions array
        void dummySet( vulkan_camera, const void * const, uint32_t )
        {}

        // Defining local types for use with the function arrays below
        using setFunction = void(*)( vulkan_camera, const void * const, uint32_t );
        using getFunction = void(*)( vulkan_camera, void * const, uint32_t );

        // Setting up the function pointers for the set function array
        constexpr setFunction setFunctions[]
        {
            setUpDirection,
            setFieldOfView,
            setAspectRatio,
            setViewWidth,
            setViewHeight,
            setNearPlane,
            setFarPlane,
            dummySet,
            dummySet,
            dummySet,
            dummySet,
            dummySet,
            dummySet,
            dummySet,
        };
        // Adding a check such that the arrays match
        static_assert( _countof( setFunctions ) == camera_parameter::count );

        // Setting up the function pointers for the get function array
        constexpr getFunction getFunctions[]
        {
            getUpDirection,
            getFieldOfView,
            getAspectRatio,
            getViewWidth,
            getViewHeight,
            getNearPlane,
            getFarPlane,
            getView,
            getProjection,
            getInverseProjection,
            getViewProjection,
            getInverseViewProjection,
            getProjectionType,
            getEntityId,
        };
        // Adding a check such that the arrays match
        static_assert( _countof( getFunctions ) == camera_parameter::count );

        // Local list of all enabled cameras
        utils::free_list<vulkan_camera>     cameras;

    } // anonymous namespace
    
    vulkan_camera::vulkan_camera( camera_init_info info )
     :
        m_UpDirection( info.upDirection ),
        m_NearPlane( info.nearPlane ),
        m_FarPlane( info.farPlane ),
        m_Fov( info.fov ),
        m_AspectRatio( info.aspectRatio ),
        m_ProjectionType( info.type ),
        m_EntityId( info.entityId ),
        m_IsDirty( true )
    {
        assert( id::isValid( m_EntityId ) );
        update( );
    }

    void vulkan_camera::update( void )
    {
        game_entity::entity entity { game_entity::entity_id{ m_EntityId } };
        math::fv3d pos { entity.getTransform().getPosition() };
        math::fv3d dir { entity.getTransform().getOrientation() };

        // Get view matrix via direction
        m_View = math::viewLookToVkRH( pos, dir, m_UpDirection );

        // Update perspective/orthographic projection if anything
        // changed since last time
        if ( m_IsDirty )
        {
            if ( graphics::camera::perspective == m_ProjectionType )
            {
                // Calculate projection matrix
                m_Projection = math::perspectiveVkRH( math::toRadians( m_Fov ),
                                                      m_AspectRatio,
                                                      m_NearPlane,
                                                      m_FarPlane );
            }
            else
            {
                // Calculate orthographics matrix
                m_Projection = math::orthographicVkRH( m_ViewWidth,
                                                       m_ViewHeight,
                                                       m_NearPlane,
                                                       m_FarPlane );
            }
            // TODO(klek): Implement the matrix inverse function in math
            //m_InverseProjection = math::matrixInverse( m_Projection );
            m_IsDirty = false;
        }

        m_ViewProjection = m_View * m_Projection;
        // TODO(klek): Implement the matrix inverse function in math
        //m_InverseViewProjection = math::matrixInverse( m_ViewProjection );
    }

    void vulkan_camera::setUpDirection( math::fv3d upDir )
    {
        m_UpDirection = upDir;
    }

    void vulkan_camera::setFieldOfView( float fov )
    {
        assert( m_ProjectionType == graphics::camera::perspective );
        m_Fov = fov;
        m_IsDirty = true;
    }

    void vulkan_camera::setAspectRatio( float aspectRatio )
    {
        assert( m_ProjectionType == graphics::camera::perspective );
        m_AspectRatio = aspectRatio;
        m_IsDirty = true;
    }

    void vulkan_camera::setViewWidth( float width )
    {
        assert( width );
        assert( m_ProjectionType == graphics::camera::orthographic );
        m_ViewWidth = width;
        m_IsDirty = true;
    }

    void vulkan_camera::setViewHeight( float height )
    {
        assert( height );
        assert( m_ProjectionType == graphics::camera::orthographic );
        m_ViewHeight = height;
        m_IsDirty = true;
    }

    void vulkan_camera::setNearPlane( float nearPlane )
    {
        m_NearPlane = nearPlane;
        m_IsDirty = true;
    }

    void vulkan_camera::setFarPlane( float farPlane )
    {
        m_FarPlane = farPlane;
        m_IsDirty = true;
    }

    graphics::camera create( camera_init_info info )
    {
        return graphics::camera { camera_id { cameras.add( info ) } };
    }

    void remove( camera_id id )
    {
        assert( id::isValid( id ) );
        cameras.remove( id );
    }

    // The setParameter function uses a local array called setFunctions
    // which contains function-pointers to all currently implemented
    // set functions. We simply index into this array with the 
    // parameter provided for the requested function
    void setParameter( camera_id id, camera_parameter::parameter param,
                       const void * const data, uint32_t dataSize )
    {
        assert( data && dataSize );
        assert( param < camera_parameter::count );
        vulkan_camera& camera { getCamera( id ) };
        setFunctions[ param ]( camera, data, dataSize );
    }

    // The getParameter function uses a local array called getFunctions
    // which contains function-pointers to all currently implemented
    // get functions. We simply index into this array with the 
    // parameter provided for the requested function
    void getParameter( camera_id id, camera_parameter::parameter param,
                       void * const data, uint32_t dataSize )
    {
        assert( data && dataSize );
        assert( param < camera_parameter::count );
        vulkan_camera& camera { getCamera( id ) };
        getFunctions[ param ]( camera, data, dataSize );
    }

    vulkan_camera& getCamera( camera_id id )
    {
        assert( id::isValid( id ) );
        return cameras[ id ];
    }


} // namespace muggy::graphics::vulkan::camera
