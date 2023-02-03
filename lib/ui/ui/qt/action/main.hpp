#ifndef __VTX_UI_QT_ACTION_APPLICATION__
#define __VTX_UI_QT_ACTION_APPLICATION__

#include "qt/application_qt.hpp"
#include "qt/dialog.hpp"
#include "qt/state/state_machine.hpp"
#include "qt/state/visualization.hpp"
#include "src/action/action_manager.hpp"
#include "src/action/base_action.hpp"
#include "src/define.hpp"
#include "src/id.hpp"
#include "src/io/struct/scene_path_data.hpp"
#include <src/tool/logger.hpp>
#include "src/worker/loader.hpp"
#include "src/worker/saver.hpp"
#include "src/worker/scene_loader.hpp"
#include "src/worker/worker_manager.hpp"

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
