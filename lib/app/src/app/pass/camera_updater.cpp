#include "app/pass/camera_updater.hpp"
#include "app/controller/animation.hpp"
#include "app/controller/freefly.hpp"
#include "app/controller/trackball.hpp"
#include "app/services.hpp"
#include <renderer/camera.hpp>
#include <renderer/renderer.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::Pass
{
	CameraUpdater::CameraUpdater( const Entity & p_ent ) : _entity( p_ent )
	{
		auto & reg = REG();

		// Update functions.
		reg.on_update<Setting::Controller>().connect<&CameraUpdater::_onControllerSetting>( this );
		reg.on_update<Renderer::Camera>().connect<&CameraUpdater::_onUpdateCamera>( this );
		HUB().connect<Events::CameraAnimationStart, &CameraUpdater::_onCameraAnimationStart>( this );
		HUB().connect<Events::CameraTransformChange, &CameraUpdater::_onUpdateCamera>( this );

		// TODO: check if needed.
		// Trigger initial settings.
		reg.patch<Setting::Controller>( _entity, [ & ]( Setting::Controller & p_setting ) {} );
	}

	CameraUpdater::~CameraUpdater()
	{
		REG().on_update<Setting::Controller>().disconnect<&CameraUpdater::_onControllerSetting>( this );
		REG().on_update<Renderer::Camera>().disconnect<&CameraUpdater::_onUpdateCamera>( this );
		HUB().disconnectAllOf( *this );
	}

	void CameraUpdater::_onControllerSetting()
	{
		const auto & settings = REG().get<Setting::Controller>( _entity );

		// Checlk if controller changed.
		if ( _controllers.empty() || _controllers.front().hash != Util::hash( settings.current ) )
		{
			// Set controller.
			switch ( settings.current )
			{
			case Setting::E_CONTROLLER::TRACKBALL: setController<Controller::Trackball>(); break;
			case Setting::E_CONTROLLER::FREEFLY: setController<Controller::Freefly>(); break;
			default: break;
			}
		}
	}

	void CameraUpdater::_onCameraAnimationStart( const Events::CameraAnimationStart & p_event )
	{
		addController<Controller::Animation>(
			CTRL_INSERTION_MODE::FRONT,
			p_event.start,
			p_event.end,
			p_event.duration,
			p_event.interpPosition,
			p_event.interpRotation
		);
	}

	void CameraUpdater::_onUpdateCamera()
	{
		auto & transform = REG().get<Util::Math::Transform>( _entity );
		auto & camera	 = REG().get<Renderer::Camera>( _entity );

		const Mat4f viewMatrix = Util::Math::lookAt(
			transform.getPosition(), transform.getPosition() + transform.getFront(), transform.getUp()
		);
		const Mat4f projectionMatrix = camera.computeProjectionMatrix( transform.getPosition() );

		RENDERER().setCamera( camera, transform.getPosition(), viewMatrix, projectionMatrix );
	}

} // namespace VTX::App::Pass
