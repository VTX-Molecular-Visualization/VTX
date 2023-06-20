#include "app/ecs/building/entity_builder.hpp"
#include "app/ecs/registry_manager.hpp"

namespace VTX::App::ECS::Building
{
	void EntityBuilder::build()
	{
		_entity = ECS::MAIN_REGISTRY().createEntity();

		for ( const EntityBuildStep & buildStep : _buildSteps )
		{
			buildStep.addComponentFunc( _entity, _extraData );
		}

		for ( const EntityBuildStep & buildStep : _buildSteps )
		{
			buildStep.setupFunc( _entity, _extraData );
		}
	}

} // namespace VTX::App::ECS::Building
