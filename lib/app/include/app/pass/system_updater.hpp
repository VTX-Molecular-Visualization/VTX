#ifndef __VTX_APP_PASS_SYSTEM_UPDATER__
#define __VTX_APP_PASS_SYSTEM_UPDATER__

#include "app/ecs.hpp"
#include "app/events.hpp"
#include "app/pass/pass_manager.hpp"
#include <renderer/types.hpp>
#include <unordered_map>

namespace VTX::App::System
{
	struct PendingSystem;
}
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
		using MapRepresentationIndex = std::map<ECS::Entity, Renderer::RepresentationIndex>;
		using MapRepresentationBonds = std::map<ECS::Entity, Renderer::RepresentationIndex>;

		SystemUpdater();
		void update( const float, const float );

	  private:
		/**
		 * @brief All system entities.
		 */
		std::vector<ECS::Entity> _entities;

		/**
		 * @brief Dirty flag to push systems to renderer at the next update.
		 * Avoid pushing systems multiple times when multiple systems changed.
		 */
		bool _needPush = false;

		/**
		 * @brief Called during the update loop to manage pending systems.
		 */
		void _pendingSystemUpdate() noexcept;

		/**
		 * @brief Current used representations.
		 */
		// TODO: use resource manager to purge unused.
		std::map<ECS::Entity, Renderer::RepresentationIndex> _representations;

		/**
		 * @brief On system loaded/destroyed events.
		 */
		void _onSystemLoaded( const Events::SystemLoad & );
		void _onSystemDestroyed( ECS::Registry &, ECS::Entity );

		/**
		 * @brief Update renderer when data changed.
		 */
		void _onUpdateTransform( ECS::Registry &, ECS::Entity );
		void _onUpdateVisibility( ECS::Registry &, ECS::Entity );
		void _onUpdateSelection( ECS::Registry &, ECS::Entity );
		void _onUpdateRepresentation( ECS::Registry &, ECS::Entity );
		void _onUpdateColor( ECS::Registry &, ECS::Entity );

		void _onUpdateRepresentationPreset( ECS::Registry &, ECS::Entity );

		void _setRepresentation();

		void _onTrajectoryDestruction( ECS::Registry &, ECS::Entity );

		/**
		 * @brief Push all systems to renderer.
		 */
		void _pushSystems();
	};
} // namespace VTX::App::Pass

#endif
