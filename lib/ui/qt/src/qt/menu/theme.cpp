#include "qt/menu/theme.hpp"
#include "qt/widget/main_window.hpp"

namespace VTX::UI::QT::Menu
{

	void Theme::_resetLayout()
	{
		WIDGET<Widget::MainWindow>()->resetLayout();
		VTX_INFO( "Layout reseted" );
	}

} // namespace VTX::UI::QT::Menu
