#ifndef __VTX_APP_ENTITY_APPLICATION_SCENE_VIEWPOINT_ENTITY__
#define __VTX_APP_ENTITY_APPLICATION_SCENE_VIEWPOINT_ENTITY__

#include "app/component/chemistry/_fwd.hpp"
#include "app/core/ecs/base_entity.hpp"
#include <util/variant.hpp>

namespace VTX::App::Entity
{
	class Viewpoint : public Core::ECS::BaseEntity
	{
	  public:
		void setup() override;
	};
} // namespace VTX::App::Entity
#endif
