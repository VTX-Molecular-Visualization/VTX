#include "base_main_window.hpp"

namespace VTX::UI::Core
{
	BaseMainWindow::BaseMainWindow() {}
	BaseMainWindow::~BaseMainWindow() {}

	BasePanel *		  BaseMainWindow::getPanel( const WidgetKey & p_key ) { return _mapPanels[ p_key ]; }
	const BasePanel * BaseMainWindow::getPanel( const WidgetKey & p_key ) const { return _mapPanels.at( p_key ); }
	void			  BaseMainWindow::referencePanel( const WidgetKey & p_key, BasePanel * const p_panel )
	{
		_mapPanels[ p_key ] = p_panel;
	}

	const WidgetKey & BaseMainWindow::findKeyFromPanel( const BasePanel * const p_panel )
	{
		std::map<WidgetKey, BasePanel *>::const_iterator it;
		for ( it = _mapPanels.cbegin(); it != _mapPanels.cend(); it++ )
		{
			if ( it->second == p_panel )
				return it->first;
		}

		return Core::INVALID_WIDGET_KEY;
	}

} // namespace VTX::UI::Core
