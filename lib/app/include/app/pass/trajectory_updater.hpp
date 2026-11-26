#ifndef VTX_APP_PASS_TRAJECTORY_UPDATER_HPP
#define VTX_APP_PASS_TRAJECTORY_UPDATER_HPP

#include "app/ecs.hpp"
#include "app/pass/pass_manager.hpp"
#include <util/event_hub.hpp>

namespace VTX::App::Pass
{
	/**
	 * @brief System that updates the trajectory buffer and handle loading threads.
	 */
	class TrajectoryUpdater final : public IPass
	{
	  public:
		/**
		 * @brief Flag that trigger update.
		 */
		static constexpr E_PENDING_UPDATE UPDATE_TRIGGERS = E_PENDING_UPDATE::TRAJECTORY;

		/**
		 * @brief Contructors.
		 */
		TrajectoryUpdater();
		~TrajectoryUpdater() override;

		/**
		 * @brief Update each frame.
		 */
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

		/**
		 * @brief On player change, update mask.
		 */
		static void _onPlayerChange( Registry &, Entity );

		/**
		 * @brief Connections.
		 */
		Util::EventHub::ScopedConnection _loaderDestroyedConnection;
		Util::EventHub::ScopedConnection _playerConstructedConnection;
		Util::EventHub::ScopedConnection _playerUpdatedConnection;
	};
} // namespace VTX::App::Pass
#endif
