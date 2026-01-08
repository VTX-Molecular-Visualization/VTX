#ifndef __VTX_UI_QT_MENU_THEME__
#define __VTX_UI_QT_MENU_THEME__

#include "ui/qt/widget/base_widget.hpp"
#include <QMenu>

namespace VTX::UI::QT::Menu
{

	class Theme : public Widget::BaseWidget<Theme, QMenu>
	{
	  public:
		Theme( QWidget * );
		~Theme();

	  private:
		QPointer<QActionGroup> _actionGroup;
		void				   _resetLayout();
	};

} // namespace VTX::UI::QT::Menu

#endif
