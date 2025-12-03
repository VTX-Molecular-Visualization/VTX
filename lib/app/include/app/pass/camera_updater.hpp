#ifndef __VTX_APP_PASS_CAMERA_UPDATER__
#define __VTX_APP_PASS_CAMERA_UPDATER__

#include "app/ecs.hpp"
#include "app/events.hpp"
#include "app/pass/pass_manager.hpp"
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
		const ECS::Entity _entity;

		// Mat4f _transform		= MAT4F_ID;
		Mat4f _viewMatrix		= MAT4F_ID;
		Mat4f _projectionMatrix = MAT4F_ID;

		/**
		 * @brief On update.
		 */
		void _onUpdate( ECS::Registry &, ECS::Entity );

		/**
		 * @brief On animation end event.
		 */
		void _onCameraAnimationEnded( const Events::CameraAnimationEnd & );
	}; // namespace VTX::App::Pass
} // namespace VTX::App::Pass

#endif
