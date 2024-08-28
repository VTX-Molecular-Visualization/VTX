#ifndef __VTX_UI_ACTION_VISUALIZATION__
#define __VTX_UI_ACTION_VISUALIZATION__

#include <app/core/action/base_action.hpp>
#include <util/collection.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Action::Visualization
{
	class ToggleCameraController final : public App::Core::Action::BaseAction
	{
	  public:
		ToggleCameraController() {}
		void execute() override;
	};
	class ChangeCameraController final : public App::Core::Action::BaseAction
	{
	  public:
		ChangeCameraController( const Util::CollectionKey & p_controllerID ) : _controllerID( p_controllerID ) {}
		void execute() override;

	  private:
		Util::CollectionKey _controllerID;
	};

	class ApplyNextCameraController final : public App::Core::Action::BaseAction
	{
	  public:
		ApplyNextCameraController() {}
		void execute() override;
	};
} // namespace VTX::App::Action::Visualization
#endif
