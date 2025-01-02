#ifndef __VTX_APP_ACTION_MODE__
#define __VTX_APP_ACTION_MODE__

#include "app/core/mode/concepts.hpp"
#include <app/application/scene.hpp>
#include <app/component/mode.hpp>
#include <app/core/action/base_action.hpp>
#include <util/collection.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Action::Mode
{
	template<Core::Mode::ConceptMode M>
	class SetMode final : public Core::Action::BaseAction
	{
	  public:
		SetMode() {}
		void execute()
		{
			Component::Mode & component = ECS_REGISTRY().getComponent<Component::Mode>( SCENE() );
			component.setMode<M>();
		}
	};
} // namespace VTX::App::Action::Mode
#endif
