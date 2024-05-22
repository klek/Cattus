//********************************************************************
//  File:    vulkanCamera.h
//  Date:    Tue, 07 May 2024: 21:18
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(VULKAN_CAMERA_H)
#define VULKAN_CAMERA_H

#include "vulkanCommon.h"

namespace muggy::graphics::vulkan::camera
{
    class vulkan_camera
    {
    public:
        explicit vulkan_camera( camera_init_info info );

        void update( void );
        // Set functions
        void setUpDirection( math::fv3d upDir );
        void setFieldOfView( float fov );
        void setAspectRatio( float aspectRatio );
        void setViewWidth( float width );
        void setViewHeight( float height );
        void setNearPlane( float nearPlane );
        void setFarPlane( float farPlane );

        // Get functions
        // TODO(klek): Fix math library to be of literal type
        [[nodiscard]] constexpr math::fv3d getUpDirection( void ) const { return m_UpDirection; }
        [[nodiscard]] constexpr float getFieldOfView( void ) const { return m_Fov; }
        [[nodiscard]] constexpr float getAspectRatio( void ) const { return m_AspectRatio; }
        [[nodiscard]] constexpr float getViewWidth( void ) const { return m_ViewWidth; }
        [[nodiscard]] constexpr float getViewHeight( void ) const { return m_ViewHeight; }
        [[nodiscard]] constexpr float getNearPlane( void ) const { return m_NearPlane; }
        [[nodiscard]] constexpr float getFarPlane( void ) const { return m_FarPlane; }
        [[nodiscard]] constexpr math::fmat4 getView( void ) const { return m_View; }
        [[nodiscard]] constexpr math::fmat4 getProjection( void ) const { return m_Projection; }
        [[nodiscard]] constexpr math::fmat4 getInverseProjection( void ) const { return m_InverseProjection; }
        [[nodiscard]] constexpr math::fmat4 getViewProjection( void ) const { return m_ViewProjection; }
        [[nodiscard]] constexpr math::fmat4 getInverseViewProjection( void ) const { return m_InverseViewProjection; }
        [[nodiscard]] constexpr graphics::camera::type getProjectionType( void ) const { return m_ProjectionType; }
        [[nodiscard]] constexpr id::id_type getEntityId( void ) const { return m_EntityId; }

    private:
        math::fmat4             m_View;
        math::fmat4             m_Projection;
        math::fmat4             m_InverseProjection;
        math::fmat4             m_ViewProjection;
        math::fmat4             m_InverseViewProjection;
        math::fv3d              m_UpDirection;
        float                   m_NearPlane;
        float                   m_FarPlane;
        graphics::camera::type  m_ProjectionType;
        id::id_type             m_EntityId;
        bool                    m_IsDirty;
        // Using unions for these fields since a camera is of a
        // specific type and thus only uses one of each field
        union
        {
            float               m_Fov;  // Used for perspective
            float               m_ViewWidth;    // Used for orthographic
        };
        union
        {
            float               m_AspectRatio;  // Used for perspective
            float               m_ViewHeight;   // Used for orthograpic
        };
    };

    graphics::camera create( camera_init_info info );
    void remove( camera_id id );
    void setParameter( camera_id id, camera_parameter::parameter param,
                       const void * const data, uint32_t dataSize );
    void getParameter( camera_id id, camera_parameter::parameter param,
                       void * const data, uint32_t dataSize );
    [[nodiscard]] vulkan_camera& getCamera( camera_id id );
} // namespace muggy::graphics::vulkan::camera


#endif