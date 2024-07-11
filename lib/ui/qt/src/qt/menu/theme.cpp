#include "qt/menu/theme.hpp"
#include "qt/main_window.hpp"

namespace VTX::UI::QT::Menu
{

	void Theme::_resetLayout()
	{
		WIDGET<MainWindow>()->resetLayout();
		VTX_DEBUG( "Layout reseted" );
	}

} // namespace VTX::UI::QT::Menu
