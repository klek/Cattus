//********************************************************************
//  File:    camera.h
//  Date:    Tue, 07 May 2024: 18:50
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(CAMERA_API_H)
#define CAMERA_API_H

#include "../code/common/common.h"

namespace muggy::graphics
{
    DEFINE_TYPED_ID( camera_id );

    class camera
    {
    public:
        enum type : uint32_t
        {
            perspective,
            orthographic
        };

        constexpr explicit camera( camera_id id ) : m_Id( id ) {}
        constexpr camera( void ) = default;
        constexpr camera_id getId( void ) const { return m_Id; }
        constexpr bool isValid( void ) const { return id::isValid( m_Id ); }

        // Set functions
        void setUpDirection( math::fv3d up ) const;
        void setFieldOfView( float fov ) const;
        void setAspectRatio( float aspectRatio ) const;
        void setViewWidth( float width ) const;
        void setViewHeight( float height ) const;
        void setRange( float nearPlane, float farPlane ) const;

        // Get functions
        math::fmat4 getView( void ) const;
        math::fmat4 getProjection( void ) const;
        math::fmat4 getInverseProjection( void ) const;
        math::fmat4 getViewProjection( void ) const;
        math::fmat4 getInverseViewProjection( void ) const;
        math::fv3d getUpDirection( void ) const;
        float getNearPlane( void ) const;
        float getFarPlane( void ) const;
        float getFieldOfView( void ) const;
        float getAspectRatio( void ) const;
        float getViewWidth( void ) const;
        float getViewHeight( void ) const;
        type getProjectionType( void ) const;
        id::id_type getEntityId( void ) const;

    private:
        camera_id   m_Id { id::invalid_id };
    };
} // namespace muggy::graphics


#endif