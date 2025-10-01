#ifndef __VTX_APP_ACTION_CONTROLLER__
#define __VTX_APP_ACTION_CONTROLLER__

#include "app/action/base_action.hpp"
#include "app/application/scene.hpp"
#include "app/component/controller.hpp"
#include "app/core/controller/concepts.hpp"

namespace VTX::App::Action::Controller
{
	template<Core::Controller::ConceptController C>
	class EnableController final : public BaseAction
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
	class DisableController final : public BaseAction
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

	class ToggleCameraController final : public BaseAction
	{
	  public:
		ToggleCameraController() {}
		void execute() override;
	};

} // namespace VTX::App::Action::Controller
#endif
