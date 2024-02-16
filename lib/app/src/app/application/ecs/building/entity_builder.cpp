#include "app/application/ecs/building/entity_builder.hpp"
#include "app/application/system/ecs_system.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Application::ECS::Building
{
	void EntityBuilder::build()
	{
		_entity = MAIN_REGISTRY().createEntity();

		for ( const BuildPass & pass : _instruction )
		{
			for ( const Step & step : pass )
			{
				step( _entity, _extraData );
			}
		}
	}

} // namespace VTX::App::Application::ECS::Building
