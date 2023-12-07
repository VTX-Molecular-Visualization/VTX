#include "app/internal/serialization/scene_serializers.hpp"
#include "app/application/ecs/component_meta_function.hpp"
#include "app/application/scene.hpp"
#include "app/component/render/camera.hpp"
#include "app/component/scene/scene_item_component.hpp"
#include "app/component/scene/transform_component.hpp"
#include "app/core/ecs/base_entity.hpp"
#include "app/core/serialization/serialization.hpp"
#include <util/math/transform.hpp>

namespace VTX::App::Internal::Serialization
{
	// Scene
	Util::JSon::Object serialize( const Application::Scene & p_scene )
	{
		Util::JSon::Array items = Util::JSon::Array();

		for ( const Core::ECS::BaseEntity entity : p_scene.getAllSceneItems() )
		{
			Util::JSon::Object							  jsonComponents = Util::JSon::Object();
			const Application::ECS::EntityInfoComponent & entityInfo
				= MAIN_REGISTRY().getComponent<Application::ECS::EntityInfoComponent>( entity );

			for ( const Application::ECS::ComponentID & componentID : entityInfo.getLinkedComponents() )
			{
				Application::ECS::ComponentMetaFunction::serializeComponent( entity, componentID, jsonComponents );
			}

			items.emplace_back( jsonComponents );
		}

		return { { "CAMERA_POSITION", SERIALIZER().serialize( p_scene.getCamera().getPosition() ) },
				 { "CAMERA_ROTATION", SERIALIZER().serialize( p_scene.getCamera().getRotation() ) },
				 { "ITEMS", items } };
	}

	void deserialize( const Util::JSon::Object & p_json, Application::Scene & p_scene )
	{
		p_scene.getCamera().setPosition( SERIALIZER().deserializeField<Vec3f>( p_json, "CAMERA_POSITION" ) );
		p_scene.getCamera().setRotation( SERIALIZER().deserializeField<Quatf>( p_json, "CAMERA_ROTATION" ) );

		const Util::JSon::Array & items = p_json[ "ITEMS" ];

		for ( const Util::JSon::Object & jsonItem : items )
		{
			const Core::ECS::BaseEntity entity = MAIN_REGISTRY().createEntity();

			for ( const Util::JSon::Object::Field & field : jsonItem )
			{
				Application::ECS::ComponentMetaFunction::deserializeComponent( field.first, entity, field.second );
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

	// TransformComponent
	Util::JSon::Object serialize( const Component::Scene::Transform & p_component )
	{
		return { { "TRANSFORM", SERIALIZER().serialize( p_component.getTransform() ) } };
	}
	void deserialize( const Util::JSon::Object & p_json, Component::Scene::Transform & p_component )
	{
		p_component.setTransform( SERIALIZER().deserializeField<Util::Math::Transform>( p_json, "TRANSFORM" ) );
	}
} // namespace VTX::App::Internal::Serialization
