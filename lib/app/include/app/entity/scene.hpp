#ifndef __VTX_APP_ENTITY_APPLICATION_SCENE_ENTITY__
#define __VTX_APP_ENTITY_APPLICATION_SCENE_ENTITY__

#include "app/core/ecs/base_entity.hpp"
#include <util/variant.hpp>

namespace VTX::App::Entity
{
	class Scene : public Core::ECS::BaseEntity
	{
	  public:
		void setup() override;
	};

} // namespace VTX::App::Entity
#endif
