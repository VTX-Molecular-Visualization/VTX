#ifndef __VTX_UI_ACTION_MAIN__
#define __VTX_UI_ACTION_MAIN__

#include <app/old/component/chemistry/molecule.hpp>
#include <app/old/component/chemistry/secondary_structure.hpp>
#include <app/old/component/chemistry/solvent_excluded_surface.hpp>
#include <app/old/component/video/path.hpp>
#include <app/old/core/action/base_action.hpp>
#include <app/old/core/event/vtx_event.hpp>
#include <app/old/event.hpp>
#include <app/old/event/global.hpp>
#include <app/old/internal/io/serialization/image_export.hpp>
#include <string>
#include <util/types.hpp>
#include <vector>

namespace VTX::Action::Main
{
	class ChangeSelectionGranularity : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeSelectionGranularity( const VTX::App::Old::Application::Selection::GRANULARITY & p_granularity ) :
			_granularity( p_granularity )
		{
		}

		virtual void execute() override;

	  private:
		const VTX::App::Old::Application::Selection::GRANULARITY _granularity;
	};
	class ChangePicker : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangePicker( const App::Old::VTX_ID & p_pickerController, const int p_mode = -1 ) :
			_pickerController( p_pickerController ), _mode( p_mode )
		{
		}

		virtual void execute() override;

	  private:
		const App::Old::VTX_ID & _pickerController;
		const int			_mode;
	};

	class ClearConsoleInterface : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ClearConsoleInterface() {}

		virtual void execute() override { VTX_EVENT( VTX::App::Old::Event::Global::CLEAR_CONSOLE ); };
		virtual void displayUsage() override { VTX_INFO( "No parameters" ); }
	};

} // namespace VTX::Action::Main
#endif
