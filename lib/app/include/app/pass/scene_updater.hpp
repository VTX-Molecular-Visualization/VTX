#ifndef __VTX_APP_PASS_SCENE_UPDATER__
#define __VTX_APP_PASS_SCENE_UPDATER__

#include "app/ecs.hpp"
#include "app/pass/pass_manager.hpp"

namespace VTX::App::Pass
{
	/**
	 * @brief System that update scene (color layout, graphics config, aabb).
	 */
	class SceneUpdater : public IPass
	{
	  public:
		SceneUpdater( const ECS::Entity & );

		inline void update( const float, const float ) noexcept {}

	  private:
		/**
		 * @brief Scene entity.
		 */
		const ECS::Entity _entity;

		/**
		 * @brief Update the scene AABB when a system AABB is updated.
		 */
		void _onUpdateAABB( ECS::Registry &, ECS::Entity );

		/**
		 * @brief Update the scene AABB when a system transform is updated.
		 */
		void _onUpdateTransform( ECS::Registry &, ECS::Entity );

		/**
		 * @brief Recompute the scene AABB when a system is removed.
		 */
		void _onSystemDestroy( ECS::Registry &, ECS::Entity );

		/**
		 * @brief Recompute scene AABB from all systems.
		 */
		void _recomputeSceneAABB( ECS::Registry &, ECS::Entity p_excluded = ECS::InvalidEntity );

		/**
		 * @brief Update renderer when a preset is instantiated.
		 */
		void _onUpdateGraphicsConfig( ECS::Registry &, ECS::Entity );
		void _onUpdateColorLayout( ECS::Registry &, ECS::Entity );

		/**
		 * @brief Update renderer when a preset is updated.
		 */
		void _onUpdateGraphicsConfigPreset( ECS::Registry &, ECS::Entity );
		void _onUpdateColorLayoutPreset( ECS::Registry &, ECS::Entity );
	};
} // namespace VTX::App::Pass
#endif
