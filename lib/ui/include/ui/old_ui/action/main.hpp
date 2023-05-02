#ifndef __VTX_UI_ACTION_MAIN__
#define __VTX_UI_ACTION_MAIN__

#include <app/component/chemistry/molecule.hpp>
#include <app/component/chemistry/secondary_structure.hpp>
#include <app/component/chemistry/solvent_excluded_surface.hpp>
#include <app/component/video/path.hpp>
#include <app/core/action/base_action.hpp>
#include <app/core/event/vtx_event.hpp>
#include <app/event.hpp>
#include <app/event/global.hpp>
#include <app/internal/io/serialization/image_export.hpp>
#include <string>
#include <util/types.hpp>
#include <vector>

namespace VTX::Action::Main
{
	class ChangeSelectionGranularity : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeSelectionGranularity( const VTX::App::Application::Selection::GRANULARITY & p_granularity ) :
			_granularity( p_granularity )
		{
		}

		virtual void execute() override;

	  private:
		const VTX::App::Application::Selection::GRANULARITY _granularity;
	};
	class ChangePicker : public App::Core::Action::BaseAction
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

	class ClearConsoleInterface : public App::Core::Action::BaseAction
	{
	  public:
		explicit ClearConsoleInterface() {}

		virtual void execute() override { VTX_EVENT( VTX::App::Event::Global::CLEAR_CONSOLE ); };
		virtual void displayUsage() override { VTX_INFO( "No parameters" ); }
	};

} // namespace VTX::Action::Main
#endif
