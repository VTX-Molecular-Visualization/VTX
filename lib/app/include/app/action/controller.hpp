#ifndef __VTX_UI_ACTION_VISUALIZATION__
#define __VTX_UI_ACTION_VISUALIZATION__

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
} // namespace VTX::App::Action::Controller
#endif
