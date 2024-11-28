#ifndef __VTX_APP_CORE_ECS_REGISTRY__
#define __VTX_APP_CORE_ECS_REGISTRY__

#include "app/core/ecs/base_component.hpp"
#include "app/core/ecs/base_entity.hpp"
#include "app/core/ecs/concepts.hpp"
#include "app/core/ecs/view.hpp"
#include <entt/entity/helper.hpp>
#include <entt/entity/registry.hpp>
#include <map>

namespace VTX::App::Core::ECS
{
	enum class SIGNAL : int
	{
		CONSTRUCT,
		UPDATE,
		DESTROY
	};

	class Registry
	{
	  public:
		Registry() = default;

		template<ConceptEntity E, typename... Args>
		E createEntity( Args &&... p_args )
		{
			E entity = E( std::forward<Args>( p_args )... );
			entity._setEntityID( _enttRegistry.create() );
			entity.setup();
			return entity;
		}

		BaseEntity createEntity() { return BaseEntity( _enttRegistry.create() ); }
		void	   destroyEntity( const BaseEntity p_entity ) { _enttRegistry.destroy( p_entity ); }

		template<ConceptComponent C>
		inline BaseEntity getEntity( const C & p_component ) const
		{
			return entt::to_entity( *_enttRegistry.storage<C>(), p_component );
		}

		bool isValid( const BaseEntity p_entity ) { return p_entity != INVALID_ENTITY; }

		template<ConceptComponent C>
		C & addComponent( const BaseEntity & p_entity )
		{
			return _enttRegistry.emplace<C>( p_entity );
		}
		template<ConceptComponent C, typename... Args>
		C & addComponent( const BaseEntity & p_entity, Args &&... p_args )
		{
			return _enttRegistry.emplace<C>( p_entity, std::forward<Args>( p_args )... );
		}

		template<ConceptComponent C>
		void removeComponent( const BaseEntity & p_entity )
		{
			_enttRegistry.remove<C>( p_entity );
		}

		template<ConceptComponent C>
		C & getComponent( const BaseEntity & p_entity )
		{
			return _enttRegistry.get<C>( p_entity );
		}
		template<ConceptComponent C>
		const C & getComponent( const BaseEntity & p_entity ) const
		{
			return _enttRegistry.get<const C>( p_entity );
		}

		template<ConceptComponent C, ConceptComponent CFrom>
		C & getComponent( const CFrom & p_component )
		{
			static_assert( !std::is_pointer<CFrom>::value );

			BaseEntity entity = getEntity( p_component );
			return _enttRegistry.get<C>( entity );
		}
		template<ConceptComponent C, ConceptComponent CFrom>
		const C & getComponent( const CFrom & p_component ) const
		{
			static_assert( !std::is_pointer<CFrom>::value );

			BaseEntity entity = getEntity( p_component );
			return _enttRegistry.get<C>( entity );
		}

		template<ConceptComponent Type, ConceptComponent... Other>
		View<Type, Other...> findComponents() const
		{
			return View<Type, Other...>( _enttRegistry );
		}

		template<ConceptComponent C>
		bool hasComponent( const BaseEntity & p_entity ) const
		{
			return _enttRegistry.all_of<C>( p_entity );
		}
		template<ConceptComponent C1, ConceptComponent C2>
		bool hasComponent( const C2 & p_component ) const
		{
			const BaseEntity entity = getEntity( p_component );
			return hasComponent<C1>( entity );
		}

		bool isEmpty() const { return _enttRegistry.storage().begin() == _enttRegistry.storage().end(); }

		// template<ConceptComponent C>
		// entt::observer & getObservers()
		//{
		//	entt::observer observer { _enttRegistry, entt::collector.update<C>() };
		//	return std::forward( observer );
		// }

		// TODO: debug.
		template<ConceptComponent C, auto Func, typename Receiver>
		void connectSignal( const SIGNAL p_signal, Receiver * const p_receiver )
		{
			switch ( p_signal )
			{
			case SIGNAL::CONSTRUCT: _enttRegistry.on_construct<C>().template connect<Func>( p_receiver ); break;
			case SIGNAL::UPDATE: _enttRegistry.on_update<C>().template connect<Func>( p_receiver ); break;
			case SIGNAL::DESTROY: _enttRegistry.on_destroy<C>().template connect<Func>( p_receiver ); break;
			}
		}

		template<ConceptComponent C>
		void deleteAll()
		{
			for ( const auto entity : _enttRegistry.view<C>() )
			{
				_enttRegistry.destroy( entity );
			}
		}

		void clear() { _enttRegistry.clear(); }

	  private:
		entt::registry _enttRegistry = entt::registry();
	};
} // namespace VTX::App::Core::ECS

#endif
