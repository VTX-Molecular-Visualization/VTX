#include "base_main_window.hpp"

namespace VTX::UI::Core
{
	BaseMainWindow::BaseMainWindow() {}
	BaseMainWindow::~BaseMainWindow() {}

	BasePanel * BaseMainWindow::getPanel( const WidgetKey & p_key ) { return _mapPanels[ p_key ]; }
	void		BaseMainWindow::referencePanel( const WidgetKey & p_key, BasePanel * const p_panel )
	{
		_mapPanels[ p_key ] = p_panel;
	}
} // namespace VTX::UI::Core
