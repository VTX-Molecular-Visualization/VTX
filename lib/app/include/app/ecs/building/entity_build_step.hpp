#ifndef __VTX_APP_ECS_BUILDING_ENTITY_BUILD_STEP__
#define __VTX_APP_ECS_BUILDING_ENTITY_BUILD_STEP__

#include "app/ecs/core/base_entity.hpp"
#include <functional>
#include <util/variant.hpp>

namespace VTX::App::ECS::Building
{
	struct EntityBuildStep
	{
	  public:
		EntityBuildStep() {};
		EntityBuildStep( std::function<void( const ECS::Core::BaseEntity, const VariantMap & )> p_addComponentFunc,
						 std::function<void( const ECS::Core::BaseEntity, const VariantMap & )> p_setupFunc ) :
			addComponentFunc( p_addComponentFunc ),
			setupFunc( p_setupFunc ) {};

		std::function<void( const ECS::Core::BaseEntity, const VariantMap & )>			   addComponentFunc;
		std::function<void( const ECS::Core::BaseEntity, const VariantMap & p_extraData )> setupFunc;
	};

} // namespace VTX::App::ECS::Building
#endif
