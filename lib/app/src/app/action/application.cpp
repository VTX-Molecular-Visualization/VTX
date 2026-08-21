#include "app/action/application.hpp"
#include "app/ecs.hpp"
#include "app/events.hpp"
#include "app/services.hpp"
#include "app/session.hpp"
#include <renderer/renderer.hpp>
#include <util/event_hub.hpp>
#include <util/thread/thread_manager.hpp>

namespace VTX::App::Action::Application
{

	void Resize::execute( const size_t p_width, const size_t p_height, const bool p_resizeMainWindow )
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
		HUB().trigger<Events::RendererResize>( p_width, p_height, p_resizeMainWindow );
	}

	void CheckForUpdate::execute() { SESSION().checkForUpdate(); }

	void Update::execute() { SESSION().downloadUpdate(); }

	void RestartAfterUpdate::execute() { SESSION().applyDownloadedUpdate(); }

	void Quit::execute() { HUB().trigger<Events::ApplicationStop>(); }

	void SetVSync::execute( const bool p_enable ) { RENDERER().setVSync( p_enable ); }

	void SetSavePower::execute( const bool p_enable ) { RENDERER().setForceUpdate( not p_enable ); }

	void StopThread::execute( const Util::Thread::BaseThread::ID & p_id ) { THREAD().stop( p_id ); }

} // namespace VTX::App::Action::Application
