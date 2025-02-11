#ifndef __VTX_APP_ACTION_CONTROLLER__
#define __VTX_APP_ACTION_CONTROLLER__

#include "app/core/controller/concepts.hpp"
#include <app/application/scene.hpp>
#include <app/component/controller.hpp>
#include <app/core/action/base_action.hpp>

namespace VTX::App::Action::Controller
{
	template<Core::Controller::ConceptController C>
	class EnableController final : public Core::Action::BaseAction
	{
	  public:
		EnableController() {}
		void execute()
		{
			Component::Controller & component
				= ECS_REGISTRY().getComponent<Component::Controller>( SCENE().getCamera() );
			component.enableController<C>();
		}
	};

	template<Core::Controller::ConceptController C>
	class DisableController final : public Core::Action::BaseAction
	{
	  public:
		DisableController() {}
		void execute()
		{
			Component::Controller & component
				= ECS_REGISTRY().getComponent<Component::Controller>( SCENE().getCamera() );
			component.disableController<C>();
		}
	};

	class ToggleCameraController final : public Core::Action::BaseAction
	{
	  public:
		ToggleCameraController() {}
		void execute() override;
	};

} // namespace VTX::App::Action::Controller
#endif
