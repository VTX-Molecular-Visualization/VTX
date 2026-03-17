#include "app/action/application.hpp"
#include "app/ecs.hpp"
#include "app/events.hpp"
#include "app/services.hpp"
#include "app/session.hpp"
#include <renderer/renderer.hpp>
#include <util/event_hub.hpp>

namespace VTX::App::Action::Application
{

	void Quit::execute() { HUB().trigger<Events::ApplicationStop>(); }

	void Resize::execute( const size_t p_width, const size_t p_height )
	{
		REG().patch<Renderer::Camera>(
			ECS::getFirstEntityOnlyWithComponents<Renderer::Camera>(),
			[ p_width, p_height ]( Renderer::Camera & p_camera )
			{
				p_camera.screenHeight = p_height;
				p_camera.screenWidth  = p_width;
			}
		);

		RENDERER().resize( p_width, p_height );
	}

	void SetSavePower::execute( const bool p_enable ) { RENDERER().setForceUpdate( not p_enable ); }

	void Update::execute() { SESSION().downloadUpdate(); }

} // namespace VTX::App::Action::Application
