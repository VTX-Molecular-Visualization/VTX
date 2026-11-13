#include "app/pass/camera_updater.hpp"
#include "app/controller/animation.hpp"
#include "app/controller/freefly.hpp"
#include "app/controller/trackball.hpp"
#include "app/pending_updates.hpp"
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
		_controllerUpdatedConnection
			= reg.on_update<Setting::Controller>().connect<&CameraUpdater::_onControllerSetting>( this );
		_cameraUpdatedConnection = reg.on_update<Renderer::Camera>().connect<&CameraUpdater::_onUpdateCamera>( this );
		_animationStartConnection
			= HUB().connect<Events::CameraAnimationStart, &CameraUpdater::_onCameraAnimationStart>( this );
		_transformChangedConnection
			= HUB().connect<Events::CameraTransformChange, &CameraUpdater::_onUpdateCamera>( this );

		// TODO: check if needed.
		// Trigger initial settings.
		reg.patch<Setting::Controller>( _entity, [ & ]( Setting::Controller & p_setting ) {} );
	}

	void CameraUpdater::_onControllerSetting()
	{
		const auto & settings = REG().get<Setting::Controller>( _entity );

		// Check if controller changed.
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
		PENDING_UPDATES().mark( E_PENDING_UPDATE::ANIMATION );
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
		PENDING_UPDATES().mark( E_PENDING_UPDATE::ANIMATION );
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

	void CameraUpdater::_syncAnimatingState()
	{
		auto &	   reg = REG();
		const bool animating
			= not _controllers.empty() && _controllers.front().hash == Util::hash<Controller::Animation>();
		const bool tagged = reg.all_of<Controller::CameraAnimating>( _entity );
		if ( animating && not tagged )
		{
			reg.emplace<Controller::CameraAnimating>( _entity );
		}
		else if ( not animating && tagged )
		{
			reg.remove<Controller::CameraAnimating>( _entity );
		}
	}

} // namespace VTX::App::Pass
