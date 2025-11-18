#ifndef __VTX_APP_PASS_SCENE_UPDATER__
#define __VTX_APP_PASS_SCENE_UPDATER__

#include "app/ecs.hpp"
#include "app/pass/pass_manager.hpp"

namespace VTX::App::Pass
{
	class SceneUpdater : public IPass
	{
	  public:
		SceneUpdater( const ECS::Entity & );

		inline void update( const float, const float ) {}

	  private:
		const ECS::Entity _entity;

		void _onUpdateAABB( ECS::Registry & p_r, ECS::Entity p_e );
	};
} // namespace VTX::App::Pass
#endif
