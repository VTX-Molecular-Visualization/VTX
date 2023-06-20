#ifndef __VTX_APP_ECS_ENTITY_APPLICATION_SCENE_ENTITY__
#define __VTX_APP_ECS_ENTITY_APPLICATION_SCENE_ENTITY__

#include "app/ecs/core/base_entity.hpp"
#include <util/variant.hpp>

namespace VTX::App::ECS::Entity::Application
{
	class SceneEntityBuilder
	{
	  public:
		static void addComponent( const ECS::Core::BaseEntity & p_entity, const VariantMap & p_extraData );
		static void setup( const ECS::Core::BaseEntity & p_entity, const VariantMap & p_extraData );
	};

} // namespace VTX::App::ECS::Entity::Application
#endif
