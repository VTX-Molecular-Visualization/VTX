#ifndef __VTX_UI_ACTION_MAIN__
#define __VTX_UI_ACTION_MAIN__

#include <old/action/base_action.hpp>
#include <old/event/event.hpp>
#include <old/event/event_manager.hpp>
#include <old/io/struct/image_export.hpp>
#include <old/model/molecule.hpp>
#include <old/model/path.hpp>
#include <old/model/secondary_structure.hpp>
#include <old/model/solvent_excluded_surface.hpp>
#include <old/worker/snapshoter.hpp>
#include <old/worker/worker_manager.hpp>
#include <string>
#include <util/types.hpp>
#include <vector>

namespace VTX::Action::Main
{
	class ChangeSelectionGranularity : public BaseAction
	{
	  public:
		explicit ChangeSelectionGranularity( const VTX::Selection::Granularity & p_granularity ) :
			_granularity( p_granularity )
		{
		}

		virtual void execute() override;

	  private:
		const VTX::Selection::Granularity _granularity;
	};
	class ChangePicker : public BaseAction
	{
	  public:
		explicit ChangePicker( const ID::VTX_ID & p_pickerController, const int p_mode = -1 ) :
			_pickerController( p_pickerController ), _mode( p_mode )
		{
		}

		virtual void execute() override;

	  private:
		const ID::VTX_ID & _pickerController;
		const int		   _mode;
	};

	class ClearConsoleInterface : public BaseAction
	{
	  public:
		explicit ClearConsoleInterface() {}

		virtual void execute() override { VTX_EVENT( new Event::VTXEvent( Event::Global::CLEAR_CONSOLE ) ); };
		virtual void displayUsage() override { VTX_INFO( "No parameters" ); }
	};

} // namespace VTX::Action::Main
#endif
