#include "app/pass/camera_updater.hpp"
#include "app/pass/controller/animation.hpp"
#include "app/scene/camera.hpp"
#include "app/services.hpp"
#include <renderer/facade.hpp>
#include <renderer/proxy/voxels.hpp>
#include <util/math.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::Pass
{
	CameraUpdater::CameraUpdater( const ECS::Entity & p_ent ) : _entity( p_ent )
	{
		auto & reg = REG();

		// Update functions.
		reg.on_update<Util::Math::Transform>().connect<&CameraUpdater::_onUpdate>( this );
		reg.on_update<Scene::Camera>().connect<&CameraUpdater::_onUpdate>( this );

		HUB().connect<Events::CameraAnimationEnd, &CameraUpdater::_onCameraAnimationEnded>( this );

		auto & transform = reg.get<Util::Math::Transform>( p_ent );
		auto & camera	 = reg.get<Scene::Camera>( p_ent );

		transform.setPosition( { 0, 0, 100 } );

		_cameraProxy = std::make_unique<Renderer::Proxy::Camera>(
			&_viewMatrix,
			&_projectionMatrix,
			transform.getPosition(),
			Vec2i( 0, 0 ),
			*camera.near,
			*camera.far,
			*camera.projection == Scene::Camera::PROJECTION::PERSPECTIVE
		);

		RENDERER().setProxyCamera( *_cameraProxy );

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

		static Renderer::Proxy::Voxels voxels { &mins, &maxs };
		RENDERER().setProxyVoxels( voxels );
	}

	void CameraUpdater::_onUpdate( ECS::Registry & p_r, ECS::Entity p_e )
	{
		// TODO: find a workaround for this, use custom event?
		if ( p_e != _entity )
		{
			return;
		}

		auto & transform = p_r.get<Util::Math::Transform>( p_e );
		auto & camera	 = p_r.get<Scene::Camera>( p_e );

		//_transform	= transform.computeMatrix();
		_viewMatrix = Util::Math::lookAt(
			transform.getPosition(), transform.getPosition() + transform.getFront(), transform.getUp()
		);
		_projectionMatrix = camera.computeProjectionMatrix();

		_cameraProxy->onMatrixView();
		_cameraProxy->onMatrixProjection();
	}

	void CameraUpdater::_onCameraAnimationEnded( const Events::CameraAnimationEnd & )
	{
		// TODO: logic with other controllers.
	}
} // namespace VTX::App::Pass
