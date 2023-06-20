#ifndef __VTX_APP_ECS_CORE_REGISTRY__
#define __VTX_APP_ECS_CORE_REGISTRY__

#include "app/ecs/core/base_component.hpp"
#include "app/ecs/core/base_entity.hpp"
#include <concepts>
#include <entt/entt.hpp>
#include <map>

namespace VTX::App::ECS::Core
{
	template<typename E>
	concept ECS_Entity = requires( E p_component ) { std::derived_from<E, ECS::Core::BaseEntity>; };
	template<typename C>
	concept ECS_Component = requires( C p_component ) { std::derived_from<C, ECS::Core::BaseComponent>; };

	class Registry // :: public Application::Generic::BaseLockable
	{
	  public:
		Registry() = default;

		ECS::Core::BaseEntity createEntity() { return ECS::Core::BaseEntity( _enttRegistry.create() ); }

		template<ECS_Component C>
		inline ECS::Core::BaseEntity getEntity( const C & p_component ) const
		{
			return entt::to_entity( _enttRegistry, p_component );
		}

		bool isValid( const Core::BaseEntity p_entity ) { return p_entity != INVALID_ENTITY; }

		template<ECS_Component C>
		C & addComponent( const ECS::Core::BaseEntity & p_entity )
		{
			return _enttRegistry.emplace<C>( p_entity );
		}
		template<ECS_Component C, typename... Args>
		C & addComponent( const ECS::Core::BaseEntity & p_entity, Args &&... p_args )
		{
			return _enttRegistry.emplace<C>( p_entity, std::forward<Args>( p_args )... );
		}

		template<ECS_Component C>
		void removeComponent( const ECS::Core::BaseEntity & p_entity )
		{
			_enttRegistry.remove<C>( p_entity );
		}

		template<ECS_Component C>
		C & getComponent( const ECS::Core::BaseEntity & p_entity )
		{
			return _enttRegistry.get<C>( p_entity );
		}
		template<ECS_Component C>
		const C & getComponent( const ECS::Core::BaseEntity & p_entity ) const
		{
			return _enttRegistry.get<const C>( p_entity );
		}

		// template<ECS_Component C, ECS_Component... Other>
		// entt::basic_view<entt::type_list<C>, entt::type_list<Other...>, void> getTypedComponents()
		//{
		//	return _enttRegistry.view<C, Other..., void>();
		// }

		template<ECS_Component... C>
		auto getComponents() const
		{
			return _enttRegistry.view<C...>();
		}

		template<ECS_Component C>
		bool hasComponent( ECS::Core::BaseEntity & p_entity )
		{
			return _enttRegistry.all_of<C>( p_entity );
		}

		bool   isEmpty() const { return _enttRegistry.size() == 0; }
		size_t size() const { return _enttRegistry.size(); }

		template<ECS_Component C>
		entt::observer getUpdateObserver()
		{
			entt::observer observer { _enttRegistry, entt::collector.update<C>() };
			return observer;
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
} // namespace VTX::App::ECS::Core

#endif
