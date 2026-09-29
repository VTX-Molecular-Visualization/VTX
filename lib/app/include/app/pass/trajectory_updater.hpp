#ifndef __VTX_APP_PASS_TRAJECTORY_UPDATER__
#define __VTX_APP_PASS_TRAJECTORY_UPDATER__

#include "app/ecs.hpp"
#include "app/pass/pass_manager.hpp"

namespace VTX::App::Pass
{
	/**
	 * @brief System that updates the trajectory buffer and handle loading threads.
	 */
	class TrajectoryUpdater final : public IPass
	{
	  public:
		/**
		 * @brief Contructors.
		 */
		TrajectoryUpdater();
		~TrajectoryUpdater() override;

		void update( float, float );

	  private:
		/**
		 * @brief On loader destroyed.
		 */
		void _onDestroyLoader( Registry &, Entity );

		/**
		 * @brief Stop threaded loader.
		 */
		void _stopLoader( Entity );
	};
} // namespace VTX::App::Pass
#endif
