#ifndef __VTX_APP_COMPONENT_ECS_ENTITY_INFO__
#define __VTX_APP_COMPONENT_ECS_ENTITY_INFO__

#include "app/application/ecs/component_info.hpp"
#include "app/core/ecs/base_component.hpp"
#include <vector>

namespace VTX::App::Component::ECS
{
	class EntityInfoComponent : public Core::ECS::BaseComponent
	{
	  public:
		EntityInfoComponent() : Core::ECS::BaseComponent() {}
		~EntityInfoComponent() {}

		void addLinkedComponent( const Application::ECS::ComponentStaticID & p_componentID )
		{
			_linkedComponents.emplace_back( p_componentID );
		}
		void removeLinkedComponent( const Application::ECS::ComponentStaticID & p_componentID )
		{
			_linkedComponents.erase( std::find( _linkedComponents.begin(), _linkedComponents.end(), p_componentID ) );
		}

		const std::vector<Application::ECS::ComponentStaticID> & getLinkedComponents() const
		{
			return _linkedComponents;
		}

	  private:
		std::vector<Application::ECS::ComponentStaticID> _linkedComponents
			= std::vector<Application::ECS::ComponentStaticID>();
	};
} // namespace VTX::App::Component::ECS
#endif
