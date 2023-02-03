#ifndef __VTX_UI_QT_ACTION_SELECTION__
#define __VTX_UI_QT_ACTION_SELECTION__

#include "src/action/base_action.hpp"
#include "src/model/selection.hpp"

namespace VTX::UI::QT::Action::Selection
{
	class Orient : public VTX::Action::BaseAction
	{
	  public:
		explicit Orient( const Model::Selection & p_selection ) : _selection( p_selection ) {}
		virtual void execute() override;

	  private:
		const Model::Selection & _selection;
	};
} // namespace VTX::UI::QT::Action::Selection
#endif
