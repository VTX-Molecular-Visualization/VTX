#ifndef __VTX_UI_ACTION_ANIMATION__
#define __VTX_UI_ACTION_ANIMATION__

#include "app/core/animation/orient.hpp"
#include "app/core/animation/reset_camera.hpp"
#include <app/core/action/base_action.hpp>
#include <util/math/aabb.hpp>
#include <util/types.hpp>

namespace VTX::App::Action::Animation
{
	class Orient final : public App::Core::Action::BaseAction
	{
	  private:
		inline static float ORIENT_DURATION = 0.5f;

	  public:
		Orient( const Util::Math::AABB & p_targetAABB );
		Orient( const Vec3f & p_finalPosition, const Quatf & p_finalRotation, const Vec3f & p_targetPosition );

		void execute() override;

	  private:
		const Core::Animation::OrientInfo _orientInfo;
	};
	class ResetCamera final : public App::Core::Action::BaseAction
	{
	  public:
		ResetCamera();
		void execute() override;

	  private:
		const Core::Animation::ResetInfo _resetInfo;
	};
} // namespace VTX::App::Action::Animation
#endif
