#include "app/application/ecs/component_info.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Application::ECS
{
	ComponentInfo::ComponentInfo( const StructPrivacyToken & p_token )
	{
		VTXApp::get().getSystem().referenceSystem( "COMPONENT_INFO", this );
	}
} // namespace VTX::App::Application::ECS
