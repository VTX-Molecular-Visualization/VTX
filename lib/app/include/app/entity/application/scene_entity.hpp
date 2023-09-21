#ifndef __VTX_APP_ENTITY_APPLICATION_SCENE_ENTITY__
#define __VTX_APP_ENTITY_APPLICATION_SCENE_ENTITY__

#include "app/core/ecs/base_entity.hpp"
#include <util/variant.hpp>

namespace VTX::App::Entity::Application
{
	class SceneEntityBuilder
	{
	  public:
		static void addComponent( const Core::ECS::BaseEntity & p_entity, const Util::VariantMap & p_extraData );
	};

} // namespace VTX::App::Entity::Application
#endif
