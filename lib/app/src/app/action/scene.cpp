#include "app/action/scene.hpp"
#include "app/action/action_manager.hpp"
#include "app/action/camera.hpp"
#include "app/application/scene.hpp"
#include "app/component/render/camera.hpp"
#include "app/component/render/viewpoint.hpp"
#include "app/component/scene/transform_component.hpp"
#include "app/core/ecs/ecs_system.hpp"
#include "app/ecs.hpp"
#include "app/entity/system.hpp"
#include "app/entity/viewpoint.hpp"
#include "app/new/system_metadata.hpp"
#include "app/services.hpp"
#include <core/struct/system.hpp>
#include <io/reader/system.hpp>
#include <util/logger.hpp>

namespace VTX::App::Action::Scene
{

	void LoadSystem::execute()
	{
		const auto entity = ECS_REGISTRY().createEntity<Entity::System>( _path.string(), _buffer );
		ACTION().execute<App::Action::Camera::Orient>( App::SCENE().getAABB() );
		return;

		// Create entity.
		ECS::Entity system	 = REG().create();
		auto &		metadata = REG().emplace<Component::SystemMetadata>( system );
		auto &		data	 = REG().emplace<VTX::Core::Struct::System>( system );

		// Load system.
		IO::Reader::System loader;
		// systemStruct.trajectory.setOptimized();

		if ( _buffer ) // From buffer.
		{
			VTX_DEBUG( "Path: {}", _path.string() );
			loader.readBuffer( *_buffer, _path, data );
		}
		else // From disk.
		{
			loader.readFile( _path, data );
			metadata.path = _path;
		}

		const VTX::IO::Reader::Chemfiles & chemfilesReader = loader.getChemfilesReader();
		const std::string &				   pdbId		   = chemfilesReader.getPdbIdCode();
		metadata.pdbIDCode								   = pdbId;
		const std::string systemName					   = pdbId == "" ? _path.stem().string() : pdbId;
		data.name										   = systemName; // TODO: move to metadata?
	}

	CreateViewpoint::CreateViewpoint() : CreateViewpoint( SCENE().getCamera() ) {}
	CreateViewpoint::CreateViewpoint( const Component::Render::Camera & p_camera ) :
		CreateViewpoint( p_camera.getTransform().getPosition(), p_camera.getTransform().getRotation() )
	{
	}
	void CreateViewpoint::execute()
	{
		const auto entity = ECS_REGISTRY().createEntity<Entity::Viewpoint>();

		auto & viewpoint = ECS_REGISTRY().getComponent<Component::Render::Viewpoint>( entity );
		viewpoint.setPosition( _position );
		viewpoint.setRotation( _rotation );
	}

	void ClearScene::execute() { SCENE().reset(); }

} // namespace VTX::App::Action::Scene
