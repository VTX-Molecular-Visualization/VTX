#include "app/pass/camera_updater.hpp"
#include "app/services.hpp"
#include "app/settings/settings.hpp"
#include "app/settings/settings_manager.hpp"
#include <renderer/camera.hpp>
#include <renderer/renderer.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::Pass
{
	CameraUpdater::CameraUpdater( const ECS::Entity & p_ent ) :
		_entity( p_ent ), _transform( REG().get<Util::Math::Transform>( p_ent ) ),
		_target( REG().get<Renderer::Camera>( p_ent ).target )
	{
		auto & reg		= REG();
		auto & settings = SETTINGS();

		// Update functions.
		reg.on_update<Renderer::Camera>().connect<&CameraUpdater::_onUpdate>( this );
		HUB().connect<Events::CameraTransformChange, &CameraUpdater::_onUpdate>( this );

		// Connect with settings.
		reg.patch<Renderer::Camera>(
			p_ent,
			[ & ]( Renderer::Camera & p_camera )
			{
				p_camera.near		= settings.getValue<float>( Settings::Camera::NEAR_CLIP_KEY );
				p_camera.fov		= settings.getValue<float>( Settings::Camera::FOV_KEY );
				p_camera.projection = settings.getValue<Renderer::PROJECTION>( Settings::Camera::PROJECTION_KEY );
				p_camera.far		= settings.getValue<float>( Settings::Camera::FAR_CLIP_KEY );
			}
		);
	}

	void CameraUpdater::_onUpdate()
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
