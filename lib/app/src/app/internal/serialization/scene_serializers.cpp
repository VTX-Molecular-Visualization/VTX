#include "app/internal/serialization/scene_serializers.hpp"
#include "app/application/ecs/component_meta_function.hpp"
#include "app/application/scene.hpp"
#include "app/application/system/serializer.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/trajectory.hpp"
#include "app/component/ecs/entity_info.hpp"
#include "app/component/io/molecule_metadata.hpp"
#include "app/component/render/camera.hpp"
#include "app/component/scene/scene_item_component.hpp"
#include "app/component/scene/transform_component.hpp"
#include "app/core/ecs/base_entity.hpp"
#include "app/core/player/base_player.hpp"
#include "app/core/player/players.hpp"
#include "app/internal/io/reader/molecule_loader.hpp"
#include <util/algorithm/range.hpp>
#include <util/math/range_list.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::Internal::Serialization
{
	// Scene
	Util::JSon::Object serialize( const Application::Scene & p_scene )
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

		return { { "CAMERA_POSITION", SERIALIZER().serialize( p_scene.getCamera().getTransform().getPosition() ) },
				 { "CAMERA_ROTATION", SERIALIZER().serialize( p_scene.getCamera().getTransform().getRotation() ) },
				 { "ENTITIES", entities } };
	}
	void deserialize( const Util::JSon::Object & p_json, Application::Scene & p_scene )
	{
		p_scene.getCamera().getTransform().setPosition(
			SERIALIZER().deserializeField<Vec3f>( p_json, "CAMERA_POSITION" )
		);
		p_scene.getCamera().getTransform().setRotation(
			SERIALIZER().deserializeField<Quatf>( p_json, "CAMERA_ROTATION" )
		);

		const Util::JSon::Array & items = p_json[ "ENTITIES" ];

		for ( const Util::JSon::Object & jsonItem : items )
		{
			const Core::ECS::BaseEntity entity = MAIN_REGISTRY().createEntity();

			const Util::JSon::Array & componentsArray = jsonItem[ "COMPONENTS" ];

			for ( const Util::JSon::Object & component : componentsArray )
			{
				const Application::ECS::ComponentStaticID componentID
					= SERIALIZER().deserializeField<Application::ECS::ComponentStaticID>( component, "ID" );
				const Util::JSon::Object & componentData = component[ "DATA" ];

				COMPONENT_META_FUNCTION().deserializeComponent( componentID, entity, componentData );
			}

			p_scene.referenceItem( MAIN_REGISTRY().getComponent<Component::Scene::SceneItemComponent>( entity ) );
		}
	}

	// SceneItemComponent
	Util::JSon::Object serialize( const Component::Scene::SceneItemComponent & p_component )
	{
		return { { "NAME", p_component.getName() } };
	}
	void deserialize( const Util::JSon::Object & p_json, Component::Scene::SceneItemComponent & p_component )
	{
		p_component.setName( SERIALIZER().deserializeField<std::string>( p_json, "NAME" ) );
	}

	// Chemistry::MoleculeComponent
	Util::JSon::Object serialize( const Component::Chemistry::Molecule & p_component )
	{
		return { { "PDB_ID", p_component.getPdbIdCode() },
				 { "TRANSFORM", SERIALIZER().serialize( p_component.getTransform() ) } };
	}
	void deserialize( const Util::JSon::Object & p_json, Component::Chemistry::Molecule & p_component )
	{
		p_component.setPdbIdCode( SERIALIZER().deserializeField<std::string>( p_json, "PDB_ID" ) );
		SERIALIZER().deserialize( p_json[ "TRANSFORM" ], p_component.getTransform() );
	}

	// TransformComponent
	Util::JSon::Object serialize( const Component::Scene::Transform & p_component )
	{
		return { { "TRANSFORM", SERIALIZER().serialize( p_component.getTransform() ) } };
	}
	void deserialize( const Util::JSon::Object & p_json, Component::Scene::Transform & p_component )
	{
		p_component.applyTransform( SERIALIZER().deserializeField<Util::Math::Transform>( p_json, "TRANSFORM" ) );
	}

	// MoleculeMetadata
	Util::JSon::Object serialize( const Component::IO::MoleculeMetadata & p_component )
	{
		const Component::Chemistry::Molecule & moleculeComponent
			= MAIN_REGISTRY().getComponent<Component::Chemistry::Molecule>( MAIN_REGISTRY().getEntity( p_component ) );

		return { { "PATH", SERIALIZER().serialize( p_component.path ) },
				 { "PDB_ID", p_component.pdbIDCode },
				 { "SECONDARY_STRUCTURE_FROM_FILE", p_component.isSecondaryStructureLoadedFromFile },
				 { "VISIBILITY", SERIALIZER().serialize( moleculeComponent.getAtomVisibilities() ) } };
	}
	void deserialize( const Util::JSon::Object & p_json, Component::IO::MoleculeMetadata & p_component )
	{
		p_component.path	  = SERIALIZER().deserializeField<FilePath>( p_json, "PATH" );
		p_component.pdbIDCode = SERIALIZER().deserializeField<std::string>( p_json, "PDB_ID", p_component.pdbIDCode );
		p_component.isSecondaryStructureLoadedFromFile
			= SERIALIZER().deserializeField<bool>( p_json, "SECONDARY_STRUCTURE_FROM_FILE", false );

		Internal::IO::Reader::MoleculeLoader loader = Internal::IO::Reader::MoleculeLoader();
		const FilePath						 path	= FilePath( p_component.path );

		Component::Chemistry::Molecule & moleculeComponent
			= MAIN_REGISTRY().getComponent<Component::Chemistry::Molecule>( MAIN_REGISTRY().getEntity( p_component ) );

		loader.readFile( path, moleculeComponent );

		const Component::Chemistry::AtomIndexRangeList visibilities
			= SERIALIZER().deserializeField<Component::Chemistry::AtomIndexRangeList>( p_json, "VISIBILITY" );

		moleculeComponent.setAtomVisibilities( visibilities );
	}

	// TrajectoryComponent
	Util::JSon::Object serialize( const Component::Chemistry::Trajectory & p_component )
	{
		return { { "PLAYER_NAME", p_component.getPlayer().getCollectionKey() },
				 { "PLAYER_DATA", SERIALIZER().serialize( p_component.getPlayer() ) } };
	}

	void deserialize( const Util::JSon::Object & p_json, Component::Chemistry::Trajectory & p_component )
	{
		const std::string playerName = SERIALIZER().deserializeField<std::string>( p_json, "PLAYER_NAME" );

		if ( playerName.empty() )
		{
			VTX_ERROR( "Deserialization of Trajectory Component fail." );
			return;
		}

		std::unique_ptr<App::Core::Player::BasePlayer> playerPtr
			= Util::Singleton<App::Core::Player::Players>::get().instantiateItem( playerName );
		p_component.setPlayer( playerPtr );

		SERIALIZER().deserialize( p_json[ "PLAYER_DATA" ], p_component.getPlayer() );
	}
} // namespace VTX::App::Internal::Serialization
