#ifndef __VTX_APP_PASS_SYSTEM_UPDATER__
#define __VTX_APP_PASS_SYSTEM_UPDATER__

#include "app/ecs.hpp"
#include "app/events.hpp"
#include "app/pass/pass_manager.hpp"
#include <vector>

namespace VTX::App::System
{
	struct PendingSystem;
	struct Color;
	struct Representation;
	struct Selection;
} // namespace VTX::App::System

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
		 * @brief Entities mapped to renderer indexes.
		 */
		std::unordered_map<Entity, Renderer::Desc::Handle> _systems;
		std::unordered_map<Entity, Renderer::Desc::Handle> _representations;

		/**
		 * @brief Push system data to renderer.
		 */
		void _onSystemLoad( const Events::SystemLoad & );
		void _onDestroySystem( Registry &, Entity );

		/**
		 * @brief Update system data in renderer when components are updated.
		 */
		void _onUpdateTransform( Registry &, Entity );
		void _onUpdateVisibility( Registry &, Entity );
		void _onUpdateSelection( Registry &, Entity );
		void _onUpdateRepresentation( Registry &, Entity );
		void _onUpdateColor( Registry &, Entity );
		void _onDestroyTrajectory( Registry &, Entity );

		/**
		 * @brief Push representation preset data to renderer.
		 */
		void _onConstructRepresentationPreset( Registry &, Entity );
		void _onUpdateRepresentationPreset( Registry &, Entity );
		void _onDestroyRepresentationPreset( Registry &, Entity );
	};
} // namespace VTX::App::Pass

#endif
