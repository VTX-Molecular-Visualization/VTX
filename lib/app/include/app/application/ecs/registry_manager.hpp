#ifndef __VTX_APP_APPLICATION_ECS_REGISTRY_MANAGER__
#define __VTX_APP_APPLICATION_ECS_REGISTRY_MANAGER__

#include "app/application/ecs/component_info.hpp"
#include "app/application/ecs/entity_info.hpp"
#include "app/core/ecs/registry.hpp"

namespace VTX::App::Application::ECS
{
	class RegistryManager
	{
	  private:
		using BaseEntity	= Core::ECS::BaseEntity;
		using BaseComponent = Core::ECS::BaseComponent;

	  public:
		RegistryManager() {};

		BaseEntity createEntity()
		{
			BaseEntity createdEntity = _registry.createEntity();
			_registry.addComponent<EntityInfoComponent>( createdEntity );

			return createdEntity;
		}

		template<Core::ECS::ECS_Component C>
		inline BaseEntity getEntity( const C & p_component ) const
		{
			return _registry.getEntity( p_component );
		}

		bool isValid( const BaseEntity p_entity ) { return _registry.isValid( p_entity ); }

		template<Core::ECS::ECS_Component C>
		C & addComponent( const BaseEntity & p_entity )
		{
			C & res = _registry.addComponent<C>( p_entity );

			EntityInfoComponent & infoComponent = getComponent<EntityInfoComponent>( p_entity );
			const ComponentID	  componentID	= ComponentInfo::getComponentID<C>();

			if ( componentID != INVALID_COMPONENT_ID )
				infoComponent.addLinkedComponent( componentID );

			return res;
		}
		template<Core::ECS::ECS_Component C, typename... Args>
		C & addComponent( const BaseEntity & p_entity, Args &&... p_args )
		{
			C & res = _registry.addComponent<C>( p_entity, std::forward<Args...>( p_args... ) );

			EntityInfoComponent & infoComponent = getComponent<EntityInfoComponent>( p_entity );
			infoComponent.addLinkedComponent( ComponentInfo::getComponentID<C>() );

			return res;
		}

		template<Core::ECS::ECS_Component C>
		void removeComponent( const BaseEntity & p_entity )
		{
			EntityInfoComponent & infoComponent = getComponent<EntityInfoComponent>( p_entity );
			infoComponent.removeLinkedComponent( ComponentInfo::getComponentID<C>() );

			_registry.removeComponent<C>( p_entity );
		}

		template<Core::ECS::ECS_Component C>
		C & getComponent( const BaseEntity & p_entity )
		{
			return _registry.getComponent<C>( p_entity );
		}
		template<Core::ECS::ECS_Component C>
		const C & getComponent( const BaseEntity & p_entity ) const
		{
			return _registry.getComponent<const C>( p_entity );
		}

		template<Core::ECS::ECS_Component C, Core::ECS::ECS_Component CFrom>
		C & getComponent( const CFrom & p_component )
		{
			BaseEntity entity = getEntity( p_component );
			return _registry.getComponent<C>( entity );
		}
		template<Core::ECS::ECS_Component C, Core::ECS::ECS_Component CFrom>
		const C & getComponent( const CFrom & p_component ) const
		{
			BaseEntity entity = getEntity( p_component );
			return _registry.getComponent<C>( entity );
		}

		template<Core::ECS::ECS_Component Type, Core::ECS::ECS_Component... Other>
		Core::ECS::View<Type, Other...> getComponents() const
		{
			return _registry.getComponents<Type, Other...>();
		}

		template<Core::ECS::ECS_Component C>
		bool hasComponent( const BaseEntity & p_entity ) const
		{
			return _registry.hasComponent<C>( p_entity );
		}
		template<Core::ECS::ECS_Component C1, Core::ECS::ECS_Component C2>
		bool hasComponent( const C2 & p_component ) const
		{
			const BaseEntity entity = getEntity( p_component );
			return _registry.hasComponent<C1>( p_component );
		}

		bool   isEmpty() const { return _registry.isEmpty(); }
		size_t size() const { return _registry.size(); }

		template<Core::ECS::ECS_Component C>
		void deleteAll()
		{
			_registry.deleteAll<C>();
		}
		void clear() { _registry.clear(); }

	  private:
		Core::ECS::Registry _registry = Core::ECS::Registry();
	};
} // namespace VTX::App::Application::ECS
#endif
