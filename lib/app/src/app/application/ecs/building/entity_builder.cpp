#include "app/application/ecs/building/entity_builder.hpp"
#include "app/application/registry_manager.hpp"

namespace VTX::App::Application::ECS::Building
{
	void EntityBuilder::build()
	{
		_entity = MAIN_REGISTRY().createEntity();

		for ( const EntityBuildStep & buildStep : _buildSteps )
		{
			buildStep.addComponentFunc( _entity, _extraData );
		}

		for ( const EntityBuildStep & buildStep : _buildSteps )
		{
			buildStep.setupFunc( _entity, _extraData );
		}
	}

} // namespace VTX::App::Application::ECS::Building
