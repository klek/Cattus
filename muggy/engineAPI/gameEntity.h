//********************************************************************
//  File:    gameEntity.h
//  Date:    Fri, 24 Nov 2023: 21:29
//  Version: 
//  Author:  klek
//  Notes:   
//********************************************************************

#if !defined(GAME_ENTITY_H)
#define GAME_ENTITY_H

#include "../code/components/componentsCommon.h"
#include "componentTransform.h"
#include "componentScript.h"

namespace muggy {
    namespace game_entity
    {
        DEFINE_TYPED_ID(entity_id);

        class entity
        {
        public:
            constexpr explicit entity( entity_id id ) : m_Id( id ) {}
            constexpr entity( ) : m_Id( id::invalid_id ) {}
            constexpr entity_id getId( ) const { return m_Id; }
            constexpr bool isValid( ) const { return id::isValid( m_Id ); }

            [[nodiscard]] transform::component getTransform() const;
            [[nodiscard]] script::component getScript() const;

            [[nodiscard]] math::fv4d getRotation( void ) const { return getTransform().getRotation(); }
            [[nodiscard]] math::fv3d getOrientation( void ) const { return getTransform().getOrientation(); }
            [[nodiscard]] math::fv3d getPosition( void ) const { return getTransform().getPosition(); }
            [[nodiscard]] math::fv3d getScale( void ) const { return getTransform().getScale(); }
        private:
            entity_id m_Id;
        };
    } // namespace game_entity

    namespace script
    {
        // Base class for scripts 
        class entity_script : public game_entity::entity
        {
        public:
            virtual ~entity_script( ) = default;
            //virtual ~entity_script( ) {}

            // Called when the game starts
            virtual void begin( ) {}
            // Called for each frame
            virtual void update( float dt ) {};
        protected:
            constexpr explicit entity_script( game_entity::entity e )
             : game_entity::entity( e.getId() ) {}

            void setRotation( math::fv4d rotationQuaternion ) const { setRotation( this, rotationQuaternion ); }
            void setOrientation( math::fv3d orientationVector ) const { setOrientation( this, orientationVector ); }
            void setPosition( math::fv3d position ) const { setPosition( this, position ); }
            void setScale( math::fv3d scale ) const { setScale( this, scale ); }

            static void setRotation( const game_entity::entity * const entity, math::fv4d rotationQuaternion );
            static void setOrientation( const game_entity::entity * const entity, math::fv3d orientationVector );
            static void setPosition( const game_entity::entity * const entity, math::fv3d position );
            static void setScale( const game_entity::entity * const entity, math::fv3d scale );
        };

        namespace detail
        {
            using scriptPtr = std::unique_ptr<entity_script>;
            using scriptCreator = scriptPtr(*)(game_entity::entity e);
            using stringHash = std::hash<std::string>;

            uint8_t registerScript( size_t, scriptCreator);
            scriptCreator getScriptCreator( size_t );

            template <class C>
            scriptPtr createScript( game_entity::entity e )
            {
                assert( e.isValid() );
                return std::make_unique<C>(e);
            }

        // This macro allows a user to register a script with the engine
        // such that the engine knows about this when compiling the users
        // code
        // I have removed the forward declaration of the users class:
        // class TYPE;
#define REGISTER_SCRIPT(TYPE)                                               \
            namespace {                                                     \
                const uint8_t reg_##TYPE                                    \
                { muggy::script::detail::registerScript(                    \
                            muggy::script::detail::stringHash()(#TYPE),     \
                            &muggy::script::detail::createScript<TYPE> )    \
                };                                                          \
            }

        } // namespace detail
    } // namespace script
} // namespace muggy


#endif