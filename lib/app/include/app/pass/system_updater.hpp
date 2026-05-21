#ifndef __VTX_APP_PASS_SYSTEM_UPDATER__
#define __VTX_APP_PASS_SYSTEM_UPDATER__

#include "app/ecs.hpp"
#include "app/events.hpp"
#include "app/pass/pass_manager.hpp"
#include <core/struct/topology.hpp>
#include <map>
#include <renderer/types.hpp>
#include <unordered_map>
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
		void update( const float, const float );

	  private:
		/**
		 * @brief All system entities.
		 */
		std::vector<ECS::Entity> _entities;
		std::vector<ECS::Entity> _pushedEntities;

		/**
		 * @brief Dirty flag to push systems to renderer at the next update.
		 * Avoid pushing systems multiple times when multiple systems changed.
		 */
		bool _needPush = false;
		/** @brief Current used representations.
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

		void									   _setRepresentation();
		Renderer::MapRepresentationRanges		   _buildRepresentationRanges( const System::Representation & );
		std::vector<Renderer::RepresentationIndex> _buildAtomRepresentations(
			const Core::Struct::Topology &,
			const Renderer::MapRepresentationRanges &
		) const;
		std::vector<Renderer::ColorIndex> _buildAtomColors(
			const System::Color &,
			const Core::Struct::Topology &
		) const;
		std::vector<Renderer::Flag> _buildAtomFlags( const System::Selection &, const size_t ) const;

		void _onTrajectoryDestruction( ECS::Registry &, ECS::Entity );

		/**
		 * @brief Push all systems to renderer.
		 */
		void _pushSystems();
	};
} // namespace VTX::App::Pass

#endif
