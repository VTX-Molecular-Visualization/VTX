#include "app/application/ecs/entity_director.hpp"
#include <memory>

namespace VTX::App::Application::ECS
{
	EntityDirector::EntityDirector()  = default;
	EntityDirector::~EntityDirector() = default;

	std::unique_ptr<Building::EntityBuilder> EntityDirector::generateBuilder( const EntityDirectorID & p_directorID )
	{
		std::unique_ptr<Building::EntityBuilder> builderPtr
			= std::make_unique<Building::EntityBuilder>( _mapDirectors[ p_directorID ] );
		return builderPtr;
	}
	Core::ECS::BaseEntity EntityDirector::build( const EntityDirectorID & p_directorID )
	{
		std::unique_ptr<Building::EntityBuilder> builderPtr = generateBuilder( p_directorID );
		builderPtr->build();

		return builderPtr->getEntity();
	}

	void EntityDirector::addBuildStep( const EntityDirectorID &	   p_directorID,
									   const Building::PASS_ENUM & p_pass,
									   const Building::Step &	   p_buildStep )
	{
		_mapDirectors[ p_directorID ][ int( p_pass ) ].emplace_back( p_buildStep );
	}

} // namespace VTX::App::Application::ECS
