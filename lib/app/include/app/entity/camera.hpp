#ifndef __VTX_APP_ENTITY_APPLICATION_SCENE_CAMERA_ENTITY__
#define __VTX_APP_ENTITY_APPLICATION_SCENE_CAMERA_ENTITY__

#include "app/component/render/_fwd.hpp"
#include "app/core/ecs/base_entity.hpp"
#include <util/variant.hpp>

namespace VTX::App::Entity
{
	class Camera : public Core::ECS::BaseEntity
	{
	  public:
		void setup() override;
	};

} // namespace VTX::App::Entity
#endif
