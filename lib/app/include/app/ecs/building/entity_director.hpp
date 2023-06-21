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
		inline static std::unique_ptr<EntityBuilder> generateBuilder( const EntityDirectorID & p_directorID )
		{
			std::unique_ptr<EntityBuilder> builderPtr
				= std::make_unique<EntityBuilder>( _mapDirectors[ p_directorID ] );
			return builderPtr;
		}
		inline static Core::BaseEntity launchBuilder( const EntityDirectorID & p_directorID )
		{
			std::unique_ptr<EntityBuilder> builderPtr = generateBuilder( p_directorID );
			builderPtr->build();

			return builderPtr->getEntity();
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
