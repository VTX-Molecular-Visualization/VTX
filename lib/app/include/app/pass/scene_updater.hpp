#ifndef __VTX_APP_PASS_SCENE_UPDATER__
#define __VTX_APP_PASS_SCENE_UPDATER__

#include "app/ecs.hpp"
#include "app/pass/pass_manager.hpp"
#include <util/event_hub.hpp>

namespace VTX::App::Pass
{
	/**
	 * @brief System that update scene (color layout, graphics config, aabb).
	 */
	class SceneUpdater : public IPass
	{
	  public:
		SceneUpdater( const Entity & );

		void update( const float, const float ) noexcept {}

	  private:
		/**
		 * @brief Scene entity.
		 */
		const Entity _entity;

		/**
		 * @brief Update the scene AABB when a system AABB is updated.
		 */
		void _onUpdateAABB( Registry &, Entity );

		/**
		 * @brief Update the scene AABB when a system transform is updated.
		 */
		void _onUpdateTransform( Registry &, Entity );

		/**
		 * @brief Recompute the scene AABB when a scene item is removed.
		 */
		void _onSceneItemDestroy( Registry &, Entity );

		/**
		 * @brief Recompute scene AABB from all scene items.
		 */
		void _recomputeSceneAABB( Registry &, Entity p_excluded = InvalidEntity );

		/**
		 * @brief Update renderer when a preset is instantiated.
		 */
		void _onUpdateGraphicsConfig( Registry &, Entity );
		void _onUpdateColorLayout( Registry &, Entity );

		/**
		 * @brief Update renderer when a preset is updated.
		 */
		void _onUpdateGraphicsConfigPreset( Registry &, Entity );
		void _onUpdateColorLayoutPreset( Registry &, Entity );

		/**
		 * @brief Update renderer when global accessibility settings are updated.
		 */
		void _onUpdateAccessibility( Registry &, Entity );

		Util::EventHub::ScopedConnection _aabbUpdatedConnection;
		Util::EventHub::ScopedConnection _transformUpdatedConnection;
		Util::EventHub::ScopedConnection _systemDestroyedConnection;
		Util::EventHub::ScopedConnection _meshDestroyedConnection;
		Util::EventHub::ScopedConnection _graphicsConfigConstructedConnection;
		Util::EventHub::ScopedConnection _graphicsConfigUpdatedConnection;
		Util::EventHub::ScopedConnection _colorLayoutConstructedConnection;
		Util::EventHub::ScopedConnection _colorLayoutUpdatedConnection;
		Util::EventHub::ScopedConnection _graphicsConfigPresetUpdatedConnection;
		Util::EventHub::ScopedConnection _colorLayoutPresetUpdatedConnection;
		Util::EventHub::ScopedConnection _accessibilityUpdatedConnection;
	};
} // namespace VTX::App::Pass
#endif
