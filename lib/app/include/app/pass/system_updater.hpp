#ifndef __VTX_APP_PASS_SYSTEM_UPDATER__
#define __VTX_APP_PASS_SYSTEM_UPDATER__

#include "app/ecs.hpp"
#include "app/events.hpp"
#include "app/pass/pass_manager.hpp"

namespace VTX::App::Pass
{
	/**
	 * @brief System that update system and push to renderer.
	 */
	class SystemUpdater : public IPass
	{
	  public:
		SystemUpdater();
		inline void update( const float, const float ) {}

	  private:
		/**
		 * @brief System entities.
		 */
		std::vector<ECS::Entity> _entities;

		/**
		 * @brief Update renderer when data changed.
		 */
		void _onUpdateVisibility( ECS::Registry &, ECS::Entity );
		void _onUpdateSelection( ECS::Registry &, ECS::Entity );
		void _onUpdateRepresentation( ECS::Registry &, ECS::Entity );
		void _onUpdateColor( ECS::Registry &, ECS::Entity );

		/**
		 * @brief On system loaded event.
		 */
		void _onSystemLoaded( const Events::SystemLoad & );
	};
} // namespace VTX::App::Pass

#endif
