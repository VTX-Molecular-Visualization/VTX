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
		/**
		 * @brief Camera entity.
		 */
		const ECS::Entity _entity;

		/**
		 * @brief On update.
		 */
		void _onUpdate();

		/**
		 * @brief On animation end event.
		 */
		void _onCameraAnimationEnded( const Events::CameraAnimationEnd & );
	}; // namespace VTX::App::Pass
} // namespace VTX::App::Pass

#endif
