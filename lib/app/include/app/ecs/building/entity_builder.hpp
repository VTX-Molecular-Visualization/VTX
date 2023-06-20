#ifndef __VTX_APP_ECS_BUILDING_ENTITY_BUILDER__
#define __VTX_APP_ECS_BUILDING_ENTITY_BUILDER__

#include "app/ecs/building/entity_build_step.hpp"
#include "app/ecs/core/base_entity.hpp"
#include <util/variant.hpp>
#include <vector>

namespace VTX::App::ECS::Building
{
	class EntityBuilder
	{
	  public:
		EntityBuilder() {}
		EntityBuilder( const std::vector<EntityBuildStep> & p_buildSteps ) : _buildSteps( p_buildSteps ) {}

		VariantMap &	   getData() { return _extraData; }
		const VariantMap & getData() const { return _extraData; }

		const ECS::Core::BaseEntity getEntity() const { return _entity; }

		void build();

	  protected:
		VariantMap					 _extraData	 = VariantMap();
		std::vector<EntityBuildStep> _buildSteps = std::vector<EntityBuildStep>();
		ECS::Core::BaseEntity		 _entity	 = INVALID_ENTITY;
	};

} // namespace VTX::App::ECS::Building
#endif
