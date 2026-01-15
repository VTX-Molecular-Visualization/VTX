#include "app/pass/camera_updater.hpp"
#include "app/pass/controller/animation.hpp"
#include "app/services.hpp"
#include "app/settings/settings.hpp"
#include "app/settings/settings_manager.hpp"
#include <renderer/camera.hpp>
#include <renderer/renderer.hpp>
#include <util/math.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::Pass
{
	CameraUpdater::CameraUpdater( const ECS::Entity & p_ent ) : _entity( p_ent )
	{
		auto & reg		= REG();
		auto & settings = SETTINGS();

		// Update functions.
		reg.on_update<Util::Math::Transform>().connect<&CameraUpdater::_onUpdate>( this );
		reg.on_update<Renderer::Camera>().connect<&CameraUpdater::_onUpdate>( this );

		HUB().connect<Events::CameraAnimationEnd, &CameraUpdater::_onCameraAnimationEnded>( this );

		auto & transform = reg.get<Util::Math::Transform>( p_ent );
		auto & camera	 = reg.get<Renderer::Camera>( p_ent );

		// tmp.
		transform.setPosition( { 0, 0, 100 } );

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

		// TODO: remove after debug.
		static std::vector<Vec3f> mins, maxs;
		for ( float x = -100.f; x < 100.f; x += 50.f )
		{
			for ( float y = -100.f; y < 100.f; y += 50.f )
			{
				for ( float z = -100.f; z < 100.f; z += 50.f )
				{
					mins.emplace_back( x, y, z );
					maxs.emplace_back( x + 50.f, y + 50.f, z + 50.f );
				}
			}
		}

		RENDERER().setVoxels( mins, maxs );
	}

	void CameraUpdater::_onUpdate( ECS::Registry & p_r, ECS::Entity p_e )
	{
		// TODO: find a workaround for this, use custom event?
		if ( p_e != _entity )
		{
			return;
		}

		auto & transform = p_r.get<Util::Math::Transform>( p_e );
		auto & camera	 = p_r.get<Renderer::Camera>( p_e );

		const Mat4f viewMatrix = Util::Math::lookAt(
			transform.getPosition(), transform.getPosition() + transform.getFront(), transform.getUp()
		);
		const Mat4f projectionMatrix = camera.computeProjectionMatrix();

		RENDERER().setCamera( camera, transform.getPosition(), viewMatrix, projectionMatrix );
	}

	void CameraUpdater::_onCameraAnimationEnded( const Events::CameraAnimationEnd & )
	{
		// TODO: logic with other controllers.
	}
} // namespace VTX::App::Pass
