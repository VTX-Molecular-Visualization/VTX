#ifndef __VTX_APP_APPLICATION_ECS_ENTITY_INFO__
#define __VTX_APP_APPLICATION_ECS_ENTITY_INFO__

#include "app/application/ecs/component_info.hpp"
#include "app/core/ecs/base_component.hpp"
#include <vector>

namespace VTX::App::Application::ECS
{
	class EntityInfoComponent : public Core::ECS::BaseComponent
	{
	  public:
		EntityInfoComponent() : Core::ECS::BaseComponent() {}
		~EntityInfoComponent() {}

		void addLinkedComponent( const ComponentID & p_componentID )
		{
			_linkedComponents.emplace_back( p_componentID );
		}
		void removeLinkedComponent( const ComponentID & p_componentID )
		{
			_linkedComponents.erase( std::find( _linkedComponents.begin(), _linkedComponents.end(), p_componentID ) );
		}

		const std::vector<ComponentID> & getLinkedComponents() const { return _linkedComponents; }

	  private:
		std::vector<ComponentID> _linkedComponents = std::vector<ComponentID>();
	};
} // namespace VTX::App::Application::ECS
#endif
