//********************************************************************
//  File:    transform.h
//  Date:    Fri, 24 Nov 2023: 21:01
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(TRANSFORM_H)
#define TRANSFORM_H

#include "componentsCommon.h"

namespace muggy::transform
{
    struct init_info
    {
        // Position in x, y, z
        float position[3];
        // Rotation in x, y, z, w (Quaternion)
        float rotation[4]{ };
        // Scale in x, y, z
        float scale[3]{ 1.0f, 1.0f, 1.0f };
    };

    struct component_flags
    {
        enum flags : uint32_t
        {
            rotation    = 0x01,
            orientation = 0x02,
            position    = 0x04,
            scale       = 0x08,

            all         = rotation | orientation | position | scale
        };
    };

    struct component_cache
    {
        math::fv4d      rotation;
        math::fv3d      orientation;
        math::fv3d      position;
        math::fv3d      scale;

        transform_id    id;
        uint32_t        flags;
    };

    component createTransform( const init_info& info, 
                               game_entity::entity entity );
    void removeTransform( component c );

    void getUpdatedComponentFlags( const game_entity::entity_id * const ids,
                                   uint32_t count, uint8_t * const flags );
    void update( const component_cache * const cache, uint32_t count );
} // namespace muggy::transform


#endif