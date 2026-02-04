#ifndef __VTX_APP_PASS_SYSTEM_UPDATER__
#define __VTX_APP_PASS_SYSTEM_UPDATER__

#include "app/ecs.hpp"
#include "app/events.hpp"
#include "app/pass/pass_manager.hpp"
#include <renderer/types.hpp>
#include <unordered_map>

namespace VTX::App::Pass
{
	/**
	 * @brief System that update system and push to renderer.
	 */
	class SystemUpdater : public IPass
	{
	  public:
		/**
		 * @brief Map representation entity to its current index.
		 */
		using RepresentationMap = std::unordered_map<ECS::Entity, Renderer::RepresentationIndex>;

		SystemUpdater();
		inline void update( const float, const float ) {}

	  private:
		/**
		 * @brief All system entities.
		 */
		std::vector<ECS::Entity> _entities;

		/**
		 * @brief Current used representations.
		 */
		RepresentationMap _representations;

		/**
		 * @brief Update renderer when data changed.
		 */
		void _onUpdateVisibility( ECS::Registry &, ECS::Entity );
		void _onUpdateSelection( ECS::Registry &, ECS::Entity );
		void _onUpdateRepresentation( ECS::Registry &, ECS::Entity );
		void _onUpdateColor( ECS::Registry &, ECS::Entity );

		void _onUpdateRepresentationPreset( ECS::Registry &, ECS::Entity );

		/**
		 * @brief On system loaded event.
		 */
		void _onSystemLoaded( const Events::SystemLoad & );

		void _setRepresentation();
	};
} // namespace VTX::App::Pass

#endif
