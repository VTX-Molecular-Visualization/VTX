#ifndef __VTX_UI_QT_ACTION_SELECTION__
#define __VTX_UI_QT_ACTION_SELECTION__

#include <app/old/core/action/base_action.hpp>
#include <app/old/application/selection/selection.hpp>

namespace VTX::UI::QT::Action::Selection
{
	class Orient : public VTX::App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Orient( const App::Old::Application::Selection::SelectionModel & p_selection ) : _selection( p_selection ) {}
		virtual void execute() override;

	  private:
		const App::Old::Application::Selection::SelectionModel & _selection;
	};
} // namespace VTX::UI::QT::Action::Selection
#endif
