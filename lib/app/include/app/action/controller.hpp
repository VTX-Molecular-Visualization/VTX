#ifndef __VTX_UI_ACTION_CONTROLLER__
#define __VTX_UI_ACTION_CONTROLLER__

#include "app/core/controller/concepts.hpp"
#include <app/core/action/base_action.hpp>
#include <util/collection.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Action::Controller
{
	class EnableController final : public App::Core::Action::BaseAction
	{
	  public:
		EnableController( const Name p_controller ) : _controller( p_controller ) {}
		void execute() override;

	  private:
		Name _controller;
	};

	class DisableController final : public App::Core::Action::BaseAction
	{
	  public:
		DisableController( const Name p_controller ) : _controller( p_controller ) {}
		void execute() override;

	  private:
		Name _controller;
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
