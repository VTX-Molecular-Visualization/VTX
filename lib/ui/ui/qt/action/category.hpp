#ifndef __VTX_UI_QT_ACTION_CATEGORY__
#define __VTX_UI_QT_ACTION_CATEGORY__

#include "src/action/base_action.hpp"
#include "src/model/category.hpp"

namespace VTX::UI::QT::Action::Category
{
	class Orient : public VTX::Action::BaseAction
	{
	  public:
		explicit Orient( const Model::Category & p_category ) : _category( p_category ) {}
		virtual void execute() override;

	  private:
		const Model::Category & _category;
	};
} // namespace VTX::UI::QT::Action::Category
#endif
