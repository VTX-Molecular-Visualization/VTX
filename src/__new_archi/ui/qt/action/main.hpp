#ifndef __VTX_UI_QT_ACTION_APPLICATION__
#define __VTX_UI_QT_ACTION_APPLICATION__

#include "__new_archi/ui/qt/application_qt.hpp"
#include "__new_archi/ui/qt/state/state_machine.hpp"
#include "__new_archi/ui/qt/state/visualization.hpp"
#include "action/action_manager.hpp"
#include "action/base_action.hpp"
#include "define.hpp"
#include "id.hpp"
#include "tool/logger.hpp"

namespace VTX::UI::QT::Action::Main
{
	class Quit : public VTX::Action::BaseAction
	{
	  public:
		explicit Quit() {}
		virtual void execute() override;
	};

	class ToggleCameraController : public VTX::Action::BaseAction
	{
	  public:
		explicit ToggleCameraController() {}
		virtual void execute() override;
	};

	class ChangeCameraController : public VTX::Action::BaseAction
	{
	  public:
		explicit ChangeCameraController( const ID::VTX_ID & p_controllerId ) : _id( p_controllerId ) {}
		virtual void execute() override;

	  private:
		const ID::VTX_ID _id;
	};

	class ResetCameraController : public VTX::Action::BaseAction
	{
	  public:
		explicit ResetCameraController() {}
		virtual void execute() override;
	};
} // namespace VTX::UI::QT::Action::Main
#endif
