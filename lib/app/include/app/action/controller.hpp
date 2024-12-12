#ifndef __VTX_UI_ACTION_CONTROLLER__
#define __VTX_UI_ACTION_CONTROLLER__

#include "app/core/controller/concepts.hpp"
#include <app/application/scene.hpp>
#include <app/component/controller.hpp>
#include <app/core/action/base_action.hpp>
#include <util/collection.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Action::Controller
{
	template<Core::Controller::ConceptController C>
	class EnableController final : public App::Core::Action::BaseAction
	{
	  public:
		EnableController() {}
		void execute()
		{
			Component::Controller & component
				= App::ECS_REGISTRY().getComponent<App::Component::Controller>( App::SCENE().getCamera() );
			component.enableController<C>();
		}
	};

	template<Core::Controller::ConceptController C>
	class DisableController final : public App::Core::Action::BaseAction
	{
	  public:
		DisableController() {}
		void execute()
		{
			Component::Controller & component
				= App::ECS_REGISTRY().getComponent<App::Component::Controller>( App::SCENE().getCamera() );
			component.disableController<C>();
		}
	};

	class ToggleCameraController final : public App::Core::Action::BaseAction
	{
	  public:
		ToggleCameraController() {}
		void execute() override;
	};

	class SetCameraProjectionOrthographic final : public App::Core::Action::BaseAction
	{
	  public:
		SetCameraProjectionOrthographic() {}
		void execute() override;
	};

	class SetCameraProjectionPerspective final : public App::Core::Action::BaseAction
	{
	  public:
		SetCameraProjectionPerspective() {}
		void execute() override;
	};

	class ToggleCameraProjection final : public App::Core::Action::BaseAction
	{
	  public:
		ToggleCameraProjection() {}
		void execute() override;
	};
} // namespace VTX::App::Action::Controller
#endif
