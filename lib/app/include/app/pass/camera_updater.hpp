#ifndef __VTX_APP_PASS_CAMERA_UPDATER__
#define __VTX_APP_PASS_CAMERA_UPDATER__

#include "app/pass/pass_manager.hpp"
#include "app/scene/camera.hpp"
#include "app/services.hpp"
#include <renderer/facade.hpp>
#include <renderer/proxy/camera.hpp>
#include <renderer/proxy/voxels.hpp>
#include <util/constants.hpp>
#include <util/math.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::Pass
{
	/**
	 * @brief System that updates camera view and projection matrices when camera or transform components are updated.
	 */
	class CameraUpdater : public IPass
	{
	  public:
		CameraUpdater( const ECS::Entity & p_ent ) : _cameraEntity( p_ent )
		{
			auto & reg = REG();

			// Update functions.
			reg.on_update<Util::Math::Transform>().connect<&CameraUpdater::_onUpdate>( this );
			reg.on_update<Scene::Camera>().connect<&CameraUpdater::_onUpdate>( this );

			auto & transform = reg.get<Util::Math::Transform>( p_ent );
			auto & camera	 = reg.get<Scene::Camera>( p_ent );

			transform.translate( { 0, 0, 100 } );

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
			for ( float x = -100.f; x <= 100.f; x += 50.f )
			{
				for ( float y = -100.f; y <= 100.f; y += 50.f )
				{
					for ( float z = -100.f; z <= 100.f; z += 50.f )
					{
						mins.emplace_back( x, y, z );
						maxs.emplace_back( x + 50.f, y + 50.f, z + 50.f );
					}
				}
			}

			static Renderer::Proxy::Voxels voxels { &mins, &maxs };
			RENDERER().setProxyVoxels( voxels );
		}

		void update( const float p_delta, const float p_elapsed ) {}

	  private:
		const ECS::Entity _cameraEntity;

		// Mat4f _transform		= MAT4F_ID;
		Mat4f _viewMatrix		= MAT4F_ID;
		Mat4f _projectionMatrix = MAT4F_ID;

		// TODO: set as component?
		std::unique_ptr<Renderer::Proxy::Camera> _cameraProxy;

		void _onUpdate( ECS::Registry & p_r, ECS::Entity p_e )
		{
			// TODO: find a workaround for this.
			if ( p_e != _cameraEntity )
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
	}; // namespace VTX::App::Pass
} // namespace VTX::App::Pass

#endif
