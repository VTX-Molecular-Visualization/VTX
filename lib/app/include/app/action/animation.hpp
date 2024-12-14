#ifndef __VTX_UI_ACTION_ANIMATION__
#define __VTX_UI_ACTION_ANIMATION__

#include "app/animation/orient.hpp"
#include <app/core/action/base_action.hpp>
#include <util/math/aabb.hpp>
#include <util/types.hpp>

namespace VTX::App::Action::Animation
{
	class Orient final : public App::Core::Action::BaseAction
	{
	  public:
		Orient( const Util::Math::AABB & p_target ) : _target( p_target ) {}

		void execute() override;

	  private:
		const Util::Math::AABB _target;
	};

} // namespace VTX::App::Action::Animation
#endif
