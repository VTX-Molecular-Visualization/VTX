#include "app/action/scene.hpp"
#include "app/application/ecs/entity_director.hpp"
#include "app/application/scene.hpp"
#include "app/application/system/ecs_system.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/render/camera.hpp"
#include "app/component/render/viewpoint.hpp"
#include "app/entity/all_entities.hpp"
#include "app/entity/scene/molecule_entity.hpp"

namespace VTX::App::Action::Scene
{
	void LoadMolecule::execute()
	{
		for ( const FilePath & moleculePath : _paths )
		{
			const std::unique_ptr<Application::ECS::Building::EntityBuilder> entityBuilder
				= ENTITY_DIRECTOR().generateBuilder( Entity::MOLECULE_ENTITY_ID );

			// Possibility to thread build function
			entityBuilder->getData()[ "scene" ]	   = Util::VTXVariant( &SCENE() );
			entityBuilder->getData()[ "filepath" ] = Util::VTXVariant( moleculePath.string() );
			entityBuilder->build();
		}
	}

	CreateViewpoint::CreateViewpoint() : CreateViewpoint( SCENE().getCamera() ) {}
	CreateViewpoint::CreateViewpoint( const Component::Render::Camera & p_camera ) :
		CreateViewpoint( p_camera.getPosition(), p_camera.getRotation() )
	{
	}
	void CreateViewpoint::execute()
	{
		const Core::ECS::BaseEntity entity = ENTITY_DIRECTOR().build( Entity::VIEWPOINT_ENTITY_ID );

		Component::Render::Viewpoint & viewpoint = MAIN_REGISTRY().getComponent<Component::Render::Viewpoint>( entity );
		viewpoint.setPosition( _position );
		viewpoint.setRotation( _rotation );
	}

} // namespace VTX::App::Action::Scene
