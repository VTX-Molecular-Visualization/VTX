#include "ui/qt/menu/theme.hpp"
#include "ui/qt/widget/main_window.hpp"

namespace VTX::UI::QT::Menu
{

	void Theme::_resetLayout()
	{
		Core::WIDGETS::get().get<Widget::MainWindow>()->resetLayout();
		VTX_INFO( "Layout reseted" );
	}

} // namespace VTX::UI::QT::Menu
