#ifndef __VTX_UI_QT_MENU_THEME__
#define __VTX_UI_QT_MENU_THEME__

#include "qt/base_widget.hpp"
#include <QMenu>

namespace VTX::UI::QT::Menu
{

	class Theme : public BaseWidget<Theme, QMenu>
	{
	  public:
		Theme( QWidget * p_parent ) : BaseWidget<Theme, QMenu>( "Theme", p_parent )
		{
			addAction<UI::Action::Theme::System>();
			addSeparator();
			addAction<UI::Action::Theme::Light>();
			addAction<UI::Action::Theme::Dark>();
			addSeparator();
			auto * aReset = addAction<UI::Action::Theme::ResetLayout>();
			connect( aReset, &QAction::triggered, this, &Theme::_resetLayout );
		}
		virtual ~Theme() {}

	  private:
		void _resetLayout();
	};

} // namespace VTX::UI::QT::Menu

#endif
