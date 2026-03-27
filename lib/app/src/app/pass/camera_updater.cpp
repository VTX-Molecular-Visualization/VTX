#include "app/pass/camera_updater.hpp"
#include "app/controller/freefly.hpp"
#include "app/controller/trackball.hpp"
#include "app/services.hpp"
#include <renderer/camera.hpp>
#include <renderer/renderer.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::Pass
{
	CameraUpdater::CameraUpdater( const ECS::Entity & p_ent ) :
		_entity( p_ent ), _transform( REG().get<Util::Math::Transform>( p_ent ) ),
		_target( REG().get<Renderer::Camera>( p_ent ).target ), _settings( REG().get<Setting::Controller>( p_ent ) )
	{
		auto & reg = REG();

		// Update functions.
		reg.on_update<Setting::Controller>().connect<&CameraUpdater::_onControllerSetting>( this );
		reg.on_update<Renderer::Camera>().connect<&CameraUpdater::_onUpdateCamera>( this );
		HUB().connect<Events::CameraTransformChange, &CameraUpdater::_onUpdateCamera>( this );

		// Trigger initial settings.
		reg.patch<Setting::Controller>( _entity, [ & ]( Setting::Controller & p_setting ) {} );
	}

	void CameraUpdater::_onControllerSetting()
	{
		// Checlk if controller changed.
		if ( _controllers.empty() || _controllers.front().hash != Util::hash( _settings.get().current ) )
		{
			// Set controller.
			switch ( _settings.get().current )
			{
			case Setting::E_CONTROLLER::TRACKBALL: setController<Controller::Trackball>(); break;
			case Setting::E_CONTROLLER::FREEFLY: setController<Controller::Freefly>(); break;
			default: break;
			}
		}
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
