#ifndef __VTX_UI_QT_ACTION_SELECTION__
#define __VTX_UI_QT_ACTION_SELECTION__

#include <app/core/action/base_action.hpp>
#include <app/application/selection/selection.hpp>

namespace VTX::UI::QT::Action::Selection
{
	class Orient : public VTX::App::Core::Action::BaseAction
	{
	  public:
		explicit Orient( const App::Application::Selection::SelectionModel & p_selection ) : _selection( p_selection ) {}
		virtual void execute() override;

	  private:
		const App::Application::Selection::SelectionModel & _selection;
	};
} // namespace VTX::UI::QT::Action::Selection
#endif
