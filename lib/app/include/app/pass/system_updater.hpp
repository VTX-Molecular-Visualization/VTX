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
		 * @brief Called during the update loop to manage pending systems.
		 */
		void _pendingSystemUpdate() noexcept;

		/**
		 * @brief Look at a pending system to determine what kind of trajectory must be used.
		 * @param
		 */
		void _pendingSystemTopologyReady( System::PendingSystem & ) noexcept;

		/**
		 * @brief Current used representations.
		 */
		// TODO: use resource manager to purge unused.
		std::map<ECS::Entity, Renderer::RepresentationIndex> _representations;

		/**
		 * @brief On system loaded event.
		 */
		void _onSystemLoaded( const Events::SystemLoad & );

		/**
		 * @brief Update renderer when data changed.
		 */
		void _onUpdateTransform( ECS::Registry &, ECS::Entity );
		void _onUpdateFlags( ECS::Registry &, ECS::Entity );
		void _onUpdateRepresentation( ECS::Registry &, ECS::Entity );
		void _onUpdateColor( ECS::Registry &, ECS::Entity );

		void _onUpdateRepresentationPreset( ECS::Registry &, ECS::Entity );

		void _setRepresentation();
	};
} // namespace VTX::App::Pass

#endif
