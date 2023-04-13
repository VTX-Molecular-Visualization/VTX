#ifndef __VTX_UI_ACTION_MAIN__
#define __VTX_UI_ACTION_MAIN__

#include <app/old_app/action/base_action.hpp>
#include <app/old_app/event/event.hpp>
#include <app/old_app/event/event_manager.hpp>
#include <app/old_app/io/struct/image_export.hpp>
#include <app/old_app/model/molecule.hpp>
#include <app/old_app/model/path.hpp>
#include <app/old_app/model/secondary_structure.hpp>
#include <app/old_app/model/solvent_excluded_surface.hpp>
#include <app/old_app/worker/snapshoter.hpp>
#include <app/old_app/worker/worker_manager.hpp>
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
