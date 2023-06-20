#ifndef __VTX_APP_ECS_BUILDING_ENTITY_DIRECTOR__
#define __VTX_APP_ECS_BUILDING_ENTITY_DIRECTOR__

#include "app/ecs/building/entity_build_step.hpp"
#include "app/ecs/building/entity_builder.hpp"
#include <map>
#include <string>
#include <vector>

namespace VTX::App::ECS::Building
{
	using EntityDirectorID = std::string;

	class EntityDirector
	{
	  public:
		inline static EntityBuilder * generateBuilder( const EntityDirectorID & p_directorID )
		{
			EntityBuilder * const builderPtr = new EntityBuilder( _mapDirectors[ p_directorID ] );
			return builderPtr;
		}
		inline static Core::BaseEntity launchBuilder( const EntityDirectorID & p_directorID )
		{
			EntityBuilder * const builder = generateBuilder( p_directorID );
			builder->build();
			const Core::BaseEntity entity = builder->getEntity();

			delete builder;

			return entity;
		}

		inline static void addBuildStep( const EntityDirectorID & p_directorID, const EntityBuildStep & p_buildStep )
		{
			_mapDirectors[ p_directorID ].emplace_back( p_buildStep );
		}

	  private:
		inline static std::map<EntityDirectorID, std::vector<EntityBuildStep>> _mapDirectors
			= std::map<EntityDirectorID, std::vector<EntityBuildStep>>();
	};
} // namespace VTX::App::ECS::Building
#endif
