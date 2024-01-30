#ifndef __VTX_UI_ACTION_VISUALIZATION__
#define __VTX_UI_ACTION_VISUALIZATION__

#include <app/core/action/base_action.hpp>
#include <app/core/collection.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::UI::Action::Visualization
{
	class ChangeCameraController final : public App::Core::Action::BaseAction
	{
	  public:
		ChangeCameraController( const App::Core::CollectionKey & p_controllerID ) : _controllerID( p_controllerID ) {}
		void execute() override;

	  private:
		App::Core::CollectionKey _controllerID;
	};

	class ApplyNextCameraController final : public App::Core::Action::BaseAction
	{
	  public:
		ApplyNextCameraController() {}
		void execute() override;
	};
} // namespace VTX::UI::Action::Visualization
#endif
