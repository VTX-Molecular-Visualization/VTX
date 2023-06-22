#ifndef __VTX_APP_APPLICATION_ECS_ENTITY_DIRECTOR__
#define __VTX_APP_APPLICATION_ECS_ENTITY_DIRECTOR__

#include "app/application/ecs/building/entity_build_step.hpp"
#include "app/application/ecs/building/entity_builder.hpp"
#include <map>
#include <string>
#include <vector>

namespace VTX::App::Application::ECS
{
	using EntityDirectorID = std::string;

	class EntityDirector
	{
	  public:
		inline static std::unique_ptr<Building::EntityBuilder> generateBuilder( const EntityDirectorID & p_directorID )
		{
			std::unique_ptr<Building::EntityBuilder> builderPtr
				= std::make_unique<Building::EntityBuilder>( _mapDirectors[ p_directorID ] );
			return builderPtr;
		}
		inline static Core::ECS::BaseEntity build( const EntityDirectorID & p_directorID )
		{
			std::unique_ptr<Building::EntityBuilder> builderPtr = generateBuilder( p_directorID );
			builderPtr->build();

			return builderPtr->getEntity();
		}

		inline static void addBuildStep( const EntityDirectorID &		   p_directorID,
										 const Building::EntityBuildStep & p_buildStep )
		{
			_mapDirectors[ p_directorID ].emplace_back( p_buildStep );
		}

	  private:
		inline static std::map<EntityDirectorID, std::vector<Building::EntityBuildStep>> _mapDirectors
			= std::map<EntityDirectorID, std::vector<Building::EntityBuildStep>>();
	};
} // namespace VTX::App::Application::ECS
#endif
