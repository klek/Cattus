//********************************************************************
//  File:    engineAPI.cpp
//  Date:    Thu, 22 Feb 2024: 08:36
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#include "entityAPI.h"
#include "../code/common/common.h"
//#include "../code/components/entity.h"
//#include "../code/components/transform.h"
//#include "../code/components/script.h"


namespace muggy
{
    transform::init_info transform_component::to_init_info()
    {
        transform::init_info info{};
        // Copy the position
        memcpy( &info.position[0], &position[0], sizeof( float ) * MAX_POS_ELEMENTS );
        // Copy the scale
        memcpy( &info.scale[0], &scale[0], sizeof( float ) * MAX_SCALE_ELEMENTS );
        // Convert euler rotation into quaternion
        math::fquat q{ math::toQuaternionFromEuler( rotation[0],
                                                    rotation[1],
                                                    rotation[2] ) };
        // Store the vector as an array
        float quat_arr[MAX_QUAT_ELEMENTS] = { q.x, q.y, q.z, q.scalar };
        // Copy the converted rotation
        memcpy( &info.rotation[0], &quat_arr[0], sizeof( float ) * MAX_QUAT_ELEMENTS );

        return info;
    }

    //****************************************************************
    // Exported functions for the entity handling
    script::init_info script_component::to_init_info()
    {
        script::init_info info{};
        info.scriptCreator = script_creator;
        return info;
    }
    
    MUGGY_API
    id::id_type createGameEntity( game_entity_descriptor* e )
    {
        assert( e );
        game_entity_descriptor& desc{ *e };

        // Create the transform info structure
        transform::init_info transform_info { desc.transform.to_init_info() };

        // Create the script info structure
        script::init_info script_info{ desc.script.to_init_info() };

        // Create the game_entity info structure based on the previous
        // components
        game_entity::entity_info entity_info
        {
            &transform_info,
            &script_info,
        };
        
        // Create the game entity and return its id
        return game_entity::createGameEntity( entity_info ).getId();
    }
    
    MUGGY_API
    void removeGameEntity( id::id_type id )
    {
        assert( id::isValid( id ) );
        game_entity::removeGameEntity( game_entity::entity_id( id ) );
    }

    MUGGY_API
    script::detail::scriptCreator getScriptCreator( const char* name )
    {
        // TODO(klek): Check that pointers here are not null
        //return ( gameCodeDll ? script::detail::getScriptCreator( script::detail::stringHash()( name ) ) : nullptr );
        return script::detail::getScriptCreator( script::detail::stringHash()( name ) );
    }
    //****************************************************************

} // namespace muggy
