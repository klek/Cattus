//********************************************************************
//  File:    transform.cpp
//  Date:    Fri, 24 Nov 2023: 22:29
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#include "transform.h"
#include "entity.h"

namespace muggy::transform
{
    namespace
    {
        utils::vector<math::fv4d>             rotations;
        utils::vector<math::fv3d>             orientations;
        utils::vector<math::fv3d>             positions;
        utils::vector<math::fv3d>             scales;

        math::fv3d calculateOrientation( math::fv4d rotation )
        {
            math::fv4d rotationQuaternion { rotation };
            // Setting positive z-axis as front for the rotation
            math::fv4d front { 0.0f, 0.0f, 1.0f, 0.0f };
            math::fv3d orientation;
            orientation = math::rotateVector3D( front, rotationQuaternion );
            return orientation;
        }

    } // namespace anonymous
    
    component createTransform( const init_info& info, game_entity::entity e )
    {
        // DEBUG: Check that the entity is valid!
        assert( e.isValid() );
        const id::id_type entityIndex{ id::index( e.getId() ) };

        // Check if we should reuse slots or create new slots
        if ( positions.size() > entityIndex )
        {
            // Reusing available slots
            math::fv4d rotation { info.rotation };
            rotations[ entityIndex ] = rotation;
            orientations[ entityIndex ] = calculateOrientation( rotation );
            positions[ entityIndex ] = math::fv3d { info.position };
            scales[ entityIndex ] = math::fv3d { info.scale };
        }
        else 
        {
            // Add new slots
            assert( positions.size() == entityIndex );
            rotations.emplace_back( info.rotation );
            orientations.emplace_back( calculateOrientation( math::fv4d{ info.rotation } ) );
            positions.emplace_back( info.position );
            scales.emplace_back( info.scale );
        }

        //return component( transform_id{ (id::id_type)positions.size() - 1 } );
        return component{ transform_id{ e.getId() } };
    }

    void removeTransform( [[maybe_unused]] component c )
    {
        assert( c.isValid() );
    }

    math::fv4d component::getRotation() const
    {
        // DEBUG: Check that this component is valid
        assert( isValid() );
        return rotations[ id::index( m_Id )];
    }

    math::fv3d component::getOrientation() const
    {
        // DEBUG: Check that this component is valid
        assert( isValid() );
        return orientations[ id::index( m_Id )];
    }

    math::fv3d component::getPosition() const
    {
        // DEBUG: Check that this component is valid
        assert( isValid() );
        return positions[ id::index( m_Id )];
    }

    math::fv3d component::getScale() const
    {
        // DEBUG: Check that this component is valid
        assert( isValid() );
        return scales[ id::index( m_Id )];
    }

} // namespace muggy::transform
