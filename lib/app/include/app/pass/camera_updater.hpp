#ifndef __VTX_APP_PASS_CAMERA_UPDATER__
#define __VTX_APP_PASS_CAMERA_UPDATER__

#include "app/pass/pass_manager.hpp"
#include "app/scene/camera.hpp"
#include "app/services.hpp"
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
			REG().on_update<Util::Math::Transform>().connect<&CameraUpdater::_onUpdate>( this );
			REG().on_update<Scene::Camera>().connect<&CameraUpdater::_onUpdate>( this );
		}

		void update( const float p_delta, const float p_elapsed ) {}

	  private:
		const ECS::Entity _cameraEntity;

		// Mat4f _transform		= MAT4F_ID;
		Mat4f _viewMatrix		= MAT4F_ID;
		Mat4f _projectionMatrix = MAT4F_ID;

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
		}
	};
} // namespace VTX::App::Pass

#endif
