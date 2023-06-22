#ifndef __VTX_APP_ENTITY_APPLICATION_SCENE_ITEM_ENTITY__
#define __VTX_APP_ENTITY_APPLICATION_SCENE_ITEM_ENTITY__

#include "app/core/ecs/base_entity.hpp"
#include <util/variant.hpp>

namespace VTX::App::Entity::Scene
{
	class SceneItemEntityBuilder
	{
	  public:
		static void addComponent( const Core::ECS::BaseEntity & p_entity, const VariantMap & p_extraData );
		static void setup( const Core::ECS::BaseEntity & p_entity, const VariantMap & p_extraData );
		static void postSetup( const Core::ECS::BaseEntity & p_entity, const VariantMap & p_extraData );
	};

} // namespace VTX::App::Entity::Scene
#endif
