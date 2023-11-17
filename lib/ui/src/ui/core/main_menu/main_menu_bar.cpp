#include "ui/core/main_menu/main_menu_bar.hpp"

namespace VTX::UI::Core::MainMenu
{
	MainMenuTooltab & MainMenuBar::getTab( const WidgetKey & p_tabID )
	{
		std::map<WidgetKey, MainMenuTooltab *>::const_iterator it = _mapKeyTab.find( p_tabID );

		MainMenuTooltab * res;

		if ( it != _mapKeyTab.end() )
		{
			res = it->second;
		}
		else
		{
			res = _instantiateTab();
			_addTab( p_tabID, res );
		}

		return *res;
	}

	MainMenuTooltab & MainMenuBar::_getTab( const int p_index ) { return *( _tabs[ p_index ] ); }

	void MainMenuBar::_addTab( const WidgetKey & p_tabID, MainMenuTooltab * const p_tab )
	{
		_tabs.emplace_back( p_tab );
		_mapKeyTab[ p_tabID ] = p_tab;
	}

} // namespace VTX::UI::Core::MainMenu
