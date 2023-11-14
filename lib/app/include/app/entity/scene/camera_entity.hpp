#ifndef __VTX_APP_ENTITY_APPLICATION_SCENE_CAMERA_ENTITY__
#define __VTX_APP_ENTITY_APPLICATION_SCENE_CAMERA_ENTITY__

#include "app/component/render/_fwd.hpp"
#include "app/core/ecs/base_entity.hpp"
#include <util/variant.hpp>

namespace VTX::App::Entity::Scene
{
	class CameraEntityBuilder
	{
	  public:
		static void addComponent( const Core::ECS::BaseEntity & p_entity, const Util::VariantMap & p_extraData );
	};

} // namespace VTX::App::Entity::Scene
#endif
