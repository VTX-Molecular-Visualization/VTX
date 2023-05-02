#ifndef __VTX_UI_QT_ACTION_CATEGORY__
#define __VTX_UI_QT_ACTION_CATEGORY__

#include <app/core/action/base_action.hpp>
#include <app/component/chemistry/category.hpp>

namespace VTX::UI::QT::Action::Category
{
	class Orient : public VTX::App::Core::Action::BaseAction
	{
	  public:
		explicit Orient( const App::Component::Chemistry::Category & p_category ) : _category( p_category ) {}
		virtual void execute() override;

	  private:
		const App::Component::Chemistry::Category & _category;
	};
} // namespace VTX::UI::QT::Action::Category
#endif
