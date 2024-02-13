#ifndef __VTX_APP_APPLICATION_ECS_ENTITY_DIRECTOR__
#define __VTX_APP_APPLICATION_ECS_ENTITY_DIRECTOR__

#include "app/application/ecs/building/entity_builder.hpp"
#include "app/application/system/system_registration.hpp"
#include "app/core/system/base_system.hpp"
#include <map>
#include <string>

namespace VTX::App::Application::ECS
{
	using EntityDirectorID = std::string;

	class EntityDirector : public Core::System::BaseSystem
	{
	  public:
		inline static const System::SystemRegistration<EntityDirector> SYSTEM
			= System::SystemRegistration<EntityDirector>();

	  public:
		EntityDirector();
		~EntityDirector();

		std::unique_ptr<Building::EntityBuilder> generateBuilder( const EntityDirectorID & p_directorID );
		Core::ECS::BaseEntity					 build( const EntityDirectorID & p_directorID );

		void addBuildStep(
			const EntityDirectorID &	p_directorID,
			const Building::PASS_ENUM & p_pass,
			const Building::Step &		p_buildStep
		);

	  private:
		std::map<EntityDirectorID, Building::BuildInstruction> _mapDirectors
			= std::map<EntityDirectorID, Building::BuildInstruction>();
	};
} // namespace VTX::App::Application::ECS

namespace VTX::App
{
	Application::ECS::EntityDirector & ENTITY_DIRECTOR();
}

#endif
