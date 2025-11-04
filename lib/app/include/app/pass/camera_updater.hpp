#ifndef __VTX_APP_PASS_CAMERA_UPDATER__
#define __VTX_APP_PASS_CAMERA_UPDATER__

#include "app/ecs.hpp"
#include "app/pass/pass_manager.hpp"
#include <renderer/proxy/camera.hpp>
#include <util/constants.hpp>

namespace VTX::App::Pass
{
	/**
	 * @brief System that updates camera view and projection matrices when camera or transform components are updated.
	 */
	class CameraUpdater : public IPass
	{
	  public:
		CameraUpdater( const ECS::Entity & );

		inline void update( const float, const float ) {}

	  private:
		const ECS::Entity _cameraEntity;

		// Mat4f _transform		= MAT4F_ID;
		Mat4f _viewMatrix		= MAT4F_ID;
		Mat4f _projectionMatrix = MAT4F_ID;

		// TODO: set as component?
		std::unique_ptr<Renderer::Proxy::Camera> _cameraProxy;

		void _onUpdate( ECS::Registry &, ECS::Entity );
	}; // namespace VTX::App::Pass
} // namespace VTX::App::Pass

#endif
