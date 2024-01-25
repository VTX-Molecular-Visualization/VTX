#include "app/application/ecs/component_meta_function.hpp"
#include "app/application/ecs/registry_manager.hpp"
#include "app/core/serialization/serialization.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Application::ECS
{
	ComponentMetaFunction::ComponentMetaFunction( const StructPrivacyToken & p_token )
	{
		VTXApp::get().getSystem().referenceSystem( "COMPONENT_META_FUNCTION", this );
	}

	void ComponentMetaFunction::deserializeComponent(
		const ComponentID &			  p_id,
		const Core::ECS::BaseEntity & p_entity,
		const Util::JSon::Object &	  p_json
	)
	{
		// Ensure that this component is referenced in this version
		if ( _mapDeserializer.contains( p_id ) )
			_mapDeserializer.at( p_id )( MAIN_REGISTRY(), SERIALIZER(), p_json, p_entity );
	};

	const Util::JSon::Object ComponentMetaFunction::serializeComponent(
		const Core::ECS::BaseEntity & p_entity,
		const ComponentID &			  p_componentID
	)
	{
		return _mapSerializer.at( p_componentID )( p_entity, MAIN_REGISTRY(), SERIALIZER() );
	}
} // namespace VTX::App::Application::ECS
