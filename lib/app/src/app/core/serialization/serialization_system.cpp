#include "app/core/serialization/serialization_system.hpp"
#include "app/application/scene.hpp"
#include "app/component/chemistry/trajectory.hpp"
#include "app/component/io/molecule_metadata.hpp"
#include "app/component/scene/scene_item_component.hpp"
#include "app/core/player/base_player.hpp"
#include "app/core/player/loop.hpp"
#include "app/core/player/once.hpp"
#include "app/core/player/ping_pong.hpp"
#include "app/core/player/revert_loop.hpp"
#include "app/core/player/revert_once.hpp"
#include "app/core/player/stop.hpp"
#include "app/serialization/io/reader/molecule_loader.hpp"
#include "app/settings.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Core::Serialization
{
	/*
	// Version
	Util::JSon::Object SerializationSystem::serialize( const App::Core::Serialization::Version & p_version )
	{
		return { { "MAJOR", p_version.major }, { "MINOR", p_version.minor }, { "REVISION", p_version.revision } };
	}
	void SerializationSystem::deserialize(
		const Util::JSon::Object &			p_json,
		App::Core::Serialization::Version & p_version
	)
	{
		p_version.major	   = deserializeField<int>( p_json, "MAJOR" );
		p_version.minor	   = deserializeField<int>( p_json, "MINOR" );
		p_version.revision = deserializeField<int>( p_json, "REVISION" );
	}

	// VTX App
	Util::JSon::Object SerializationSystem::serialize( const VTXApp & p_app ) { throw NotImplementedException(); }
	void			   SerializationSystem::deserialize( const Util::JSon::Object & p_json, VTXApp & p_app )
	{
		throw NotImplementedException();
	}

	// Settings
	Util::JSon::Object SerializationSystem::serialize( const App::Core::Settings::SettingMap & p_settings )
	{
		Util::JSon::Object settingMapJSon = Util::JSon::Object();

		for ( const auto & pair : p_settings )
		{
			settingMapJSon.appendField( pair.first, pair.second->serialize() );
		}

		return { { "MAP", settingMapJSon } };
	}
	void SerializationSystem::deserialize(
		const Util::JSon::Object &		  p_json,
		App::Core::Settings::SettingMap & p_settings
	)
	{
		if ( !p_json.contains( "MAP" ) )
			throw( IOException( "Unreadable Setting file." ) );

		const Util::JSon::Object & settingMapJSon = p_json[ "MAP" ];

		for ( const Util::JSon::Object::Field & field : settingMapJSon )
		{
			if ( p_settings.contains( field.first ) )
			{
				try
				{
					p_settings.at( field.first )->deserialize( field.second );
				}
				catch ( const std::exception & e )
				{
					VTX_ERROR( "{}", e.what() );
					VTX_WARNING( "Unable to deserialize setting {}. Keep previous value.", field.first );
				}
			}
			else
			{
				VTX_WARNING( "Unknown setting \"{}\". Setting skipped.", field.first );
			}
		}
	}

	// Scene
	Util::JSon::Object SerializationSystem::serialize( const Application::Scene & p_scene )
	{
		Util::JSon::Array entities = Util::JSon::Array();

		for ( const Core::ECS::BaseEntity entity : p_scene.getAllSceneItems() )
		{
			Util::JSon::Array							jsonComponentsArray = Util::JSon::Array();
			const Component::ECS::EntityInfoComponent & entityInfo
				= MAIN_REGISTRY().getComponent<Component::ECS::EntityInfoComponent>( entity );

			for ( const Application::ECS::ComponentStaticID & componentID : entityInfo.getLinkedComponents() )
			{
				Util::JSon::Object jsonComponent
					= { { "ID", componentID },
						{ "DATA", COMPONENT_META_FUNCTION().serializeComponent( entity, componentID ) } };

				jsonComponentsArray.emplace_back( jsonComponent );
			}

			const Util::JSon::Object jsonComponents = { { "COMPONENTS", jsonComponentsArray } };
			entities.emplace_back( jsonComponents );
		}

		return { { "CAMERA_POSITION", serialize( p_scene.getCamera().getTransform().getPosition() ) },
				 { "CAMERA_ROTATION", serialize( p_scene.getCamera().getTransform().getRotation() ) },
				 { "ENTITIES", entities } };
	}
	void SerializationSystem::deserialize( const Util::JSon::Object & p_json, Application::Scene & p_scene )
	{
		p_scene.getCamera().getTransform().setPosition( deserializeField<Vec3f>( p_json, "CAMERA_POSITION" ) );
		p_scene.getCamera().getTransform().setRotation( deserializeField<Quatf>( p_json, "CAMERA_ROTATION" ) );

		const Util::JSon::Array & items = p_json[ "ENTITIES" ];

		for ( const Util::JSon::Object & jsonItem : items )
		{
			const Core::ECS::BaseEntity entity = MAIN_REGISTRY().createEntity();

			const Util::JSon::Array & componentsArray = jsonItem[ "COMPONENTS" ];

			for ( const Util::JSon::Object & component : componentsArray )
			{
				const Application::ECS::ComponentStaticID componentID
					= deserializeField<Application::ECS::ComponentStaticID>( component, "ID" );
				const Util::JSon::Object & componentData = component[ "DATA" ];

				COMPONENT_META_FUNCTION().deserializeComponent( componentID, entity, componentData );
			}

			p_scene.referenceItem( MAIN_REGISTRY().getComponent<Component::Scene::SceneItemComponent>( entity ) );
		}
	}

	// SceneItemComponent
	Util::JSon::Object SerializationSystem::serialize( const Component::Scene::SceneItemComponent & p_component )
	{
		return { { "NAME", p_component.getName() } };
	}
	void SerializationSystem::deserialize(
		const Util::JSon::Object &			   p_json,
		Component::Scene::SceneItemComponent & p_component
	)
	{
		p_component.setName( deserializeField<std::string>( p_json, "NAME" ) );
	}

	// Chemistry::MoleculeComponent
	Util::JSon::Object SerializationSystem::serialize( const Component::Chemistry::Molecule & p_component )
	{
		return { { "PDB_ID", p_component.getPdbIdCode() }, { "TRANSFORM", serialize( p_component.getTransform() ) } };
	}
	void SerializationSystem::deserialize(
		const Util::JSon::Object &		 p_json,
		Component::Chemistry::Molecule & p_component
	)
	{
		p_component.setPdbIdCode( deserializeField<std::string>( p_json, "PDB_ID" ) );
		deserialize( p_json[ "TRANSFORM" ], p_component.getTransform() );
	}

	// TransformComponent
	Util::JSon::Object SerializationSystem::serialize( const Component::Scene::Transform & p_component )
	{
		return { { "TRANSFORM", serialize( p_component.getTransform() ) } };
	}
	void SerializationSystem::deserialize(
		const Util::JSon::Object &	  p_json,
		Component::Scene::Transform & p_component
	)
	{
		p_component.applyTransform( deserializeField<Util::Math::Transform>( p_json, "TRANSFORM" ) );
	}

	// MoleculeMetadata
	Util::JSon::Object SerializationSystem::serialize( const Component::IO::MoleculeMetadata & p_component )
	{
		const Component::Chemistry::Molecule & moleculeComponent
			= MAIN_REGISTRY().getComponent<Component::Chemistry::Molecule>( MAIN_REGISTRY().getEntity( p_component ) );

		return { { "PATH", serialize( p_component.path ) },
				 { "PDB_ID", p_component.pdbIDCode },
				 { "SECONDARY_STRUCTURE_FROM_FILE", p_component.isSecondaryStructureLoadedFromFile },
				 { "VISIBILITY", serialize( moleculeComponent.getAtomVisibilities() ) } };
	}
	void SerializationSystem::deserialize(
		const Util::JSon::Object &		  p_json,
		Component::IO::MoleculeMetadata & p_component
	)
	{
		p_component.path	  = deserializeField<FilePath>( p_json, "PATH" );
		p_component.pdbIDCode = deserializeField<std::string>( p_json, "PDB_ID", p_component.pdbIDCode );
		p_component.isSecondaryStructureLoadedFromFile
			= deserializeField<bool>( p_json, "SECONDARY_STRUCTURE_FROM_FILE", false );

		Serialization::IO::Reader::MoleculeLoader loader = Serialization::IO::Reader::MoleculeLoader();
		const FilePath							  path	 = FilePath( p_component.path );

		Component::Chemistry::Molecule & moleculeComponent
			= MAIN_REGISTRY().getComponent<Component::Chemistry::Molecule>( MAIN_REGISTRY().getEntity( p_component ) );

		loader.readFile( path, moleculeComponent );

		const Component::Chemistry::AtomIndexRangeList visibilities
			= deserializeField<Component::Chemistry::AtomIndexRangeList>( p_json, "VISIBILITY" );

		moleculeComponent.setAtomVisibilities( visibilities );
	}
	*/

} // namespace VTX::App::Core::Serialization
