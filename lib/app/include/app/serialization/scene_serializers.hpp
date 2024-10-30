#ifndef __VTX_APP_SERIALIZATION_SCENE_SERIALIZERS__
#define __VTX_APP_SERIALIZATION_SCENE_SERIALIZERS__

#include "app/application/_fwd.hpp"
#include "app/component/chemistry/_fwd.hpp"
#include "app/component/io/_fwd.hpp"
#include "app/component/scene/_fwd.hpp"
#include <util/json/json.hpp>

namespace VTX::App::Serialization
{
	// Scene
	Util::JSon::Object serialize( const App::Application::Scene & );
	void			   deserialize( const Util::JSon::Object &, App::Application::Scene & );

	// SceneItemComponent
	Util::JSon::Object serialize( const Component::Scene::SceneItemComponent & p_component );
	void deserialize( const Util::JSon::Object & p_json, Component::Scene::SceneItemComponent & p_component );

	// TransformComponent
	Util::JSon::Object serialize( const Component::Scene::Transform & p_component );
	void			   deserialize( const Util::JSon::Object & p_json, Component::Scene::Transform & p_component );

	// MoleculeComponent
	Util::JSon::Object serialize( const Component::Chemistry::System & p_component );
	void			   deserialize( const Util::JSon::Object & p_json, Component::Chemistry::System & p_component );

	// MoleculeMetadata
	Util::JSon::Object serialize( const Component::IO::MoleculeMetadata & p_component );
	void			   deserialize( const Util::JSon::Object & p_json, Component::IO::MoleculeMetadata & p_component );

	// Trajectory
	Util::JSon::Object serialize( const Component::Chemistry::Trajectory & p_component );
	void			   deserialize( const Util::JSon::Object & p_json, Component::Chemistry::Trajectory & p_component );

} // namespace VTX::App::Serialization
#endif
