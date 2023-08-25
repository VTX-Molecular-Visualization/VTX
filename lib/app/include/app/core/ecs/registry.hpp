#ifndef __VTX_APP_CORE_ECS_REGISTRY__
#define __VTX_APP_CORE_ECS_REGISTRY__

#include "app/core/ecs/base_component.hpp"
#include "app/core/ecs/base_entity.hpp"
#include "app/core/ecs/view.hpp"
#include <concepts>
#include <entt/entity/helper.hpp>
#include <entt/entity/registry.hpp>
#include <map>

namespace VTX::App::Core::ECS
{
	template<typename E>
	concept ECS_Entity = requires( E p_component ) { std::derived_from<E, BaseEntity>; };
	template<typename C>
	concept ECS_Component = requires( C p_component ) { true; };

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

		BaseEntity createEntity() { return BaseEntity( _enttRegistry.create() ); }

		template<ECS_Component C>
		inline BaseEntity getEntity( const C & p_component ) const
		{
			return entt::to_entity( _enttRegistry, p_component );
		}

		bool isValid( const BaseEntity p_entity ) { return p_entity != INVALID_ENTITY; }

		template<ECS_Component C>
		C & addComponent( const BaseEntity & p_entity )
		{
			return _enttRegistry.emplace<C>( p_entity );
		}
		template<ECS_Component C, typename... Args>
		C & addComponent( const BaseEntity & p_entity, Args &&... p_args )
		{
			return _enttRegistry.emplace<C>( p_entity, std::forward<Args>( p_args )... );
		}

		template<ECS_Component C>
		void removeComponent( const BaseEntity & p_entity )
		{
			_enttRegistry.remove<C>( p_entity );
		}

		template<ECS_Component C>
		C & getComponent( const BaseEntity & p_entity )
		{
			return _enttRegistry.get<C>( p_entity );
		}
		template<ECS_Component C>
		const C & getComponent( const BaseEntity & p_entity ) const
		{
			return _enttRegistry.get<const C>( p_entity );
		}

		template<typename Type, typename... Other>
		View<Type, Other...> getComponents() const
		{
			return View<Type, Other...>( _enttRegistry );
		}

		template<ECS_Component C>
		bool hasComponent( BaseEntity & p_entity )
		{
			return _enttRegistry.all_of<C>( p_entity );
		}

		bool   isEmpty() const { return _enttRegistry.size() == 0; }
		size_t size() const { return _enttRegistry.size(); }

		// template<ECS_Component C>
		// entt::observer & getObservers()
		//{
		//	entt::observer observer { _enttRegistry, entt::collector.update<C>() };
		//	return std::forward( observer );
		// }

		template<ECS_Component C, auto Func, typename Receiver>
		void connectSignal( const SIGNAL p_signal, Receiver * const p_receiver )
		{
			switch ( p_signal )
			{
			case SIGNAL::CONSTRUCT: _enttRegistry.on_construct<C>().connect<Func>( p_receiver ); break;
			case SIGNAL::UPDATE: _enttRegistry.on_update<C>().connect<Func>( p_receiver ); break;
			case SIGNAL::DESTROY: _enttRegistry.on_destroy<C>().connect<Func>( p_receiver ); break;
			}
		}

		template<ECS_Component C>
		void deleteAll()
		{
			entt::basic_view view = _enttRegistry.view<C>();
			_enttRegistry.destroy( view.begin(), view.end() );
		}

	  private:
		entt::registry _enttRegistry = entt::registry();
	};
} // namespace VTX::App::Core::ECS

#endif
