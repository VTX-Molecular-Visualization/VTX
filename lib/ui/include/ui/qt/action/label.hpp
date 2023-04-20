#ifndef __VTX_UI_QT_ACTION_LABEL__
#define __VTX_UI_QT_ACTION_LABEL__

#include <app/core/action/base_action.hpp>
#include <app/model/label.hpp>
#include <unordered_set>
#include <vector>

namespace VTX::UI::QT::Action::Label
{
	class Orient : public VTX::Core::Action::BaseAction
	{
	  public:
		explicit Orient( Model::Label * const p_label ) : _labels( { p_label } ) {}
		explicit Orient( const std::unordered_set<Model::Label *> & p_labels ) : _labels( p_labels ) {}

		virtual void execute() override;

	  private:
		std::unordered_set<Model::Label *> _labels;
	};
} // namespace VTX::UI::QT::Action::Label
#endif
