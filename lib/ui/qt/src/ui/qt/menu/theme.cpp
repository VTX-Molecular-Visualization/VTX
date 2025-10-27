#include "ui/qt/menu/theme.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/widget/main_window.hpp"

namespace VTX::UI::QT::Menu
{

	void Theme::_resetLayout()
	{
		MAIN_WINDOW().resetLayout();
		VTX_INFO( "Layout reseted" );
	}

} // namespace VTX::UI::QT::Menu
