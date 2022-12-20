#ifndef __VTX_UI_CORE_MAIN_MENU_BAR__
#define __VTX_UI_CORE_MAIN_MENU_BAR__

#include "__new_archi/ui/core/define.hpp"
#include "main_menu_tooltab.hpp"
#include <map>
#include <vector>

namespace VTX::UI::Core::MainMenu
{
	class MainMenuBar
	{
	  public:
		virtual void			  setCurrentTab( int p_index ) = 0;
		virtual MainMenuTooltab & getTab( const WidgetKey & p_tabID );

	  protected:
		virtual MainMenuTooltab * _instantiateTab() const = 0;
		virtual MainMenuTooltab & _getTab( const int p_index );

		virtual void _addTab( const WidgetKey & p_tabID, MainMenuTooltab * const p_tab );

	  private:
		std::vector<MainMenuTooltab *>		   _tabs	  = std::vector<MainMenuTooltab *>();
		std::map<WidgetKey, MainMenuTooltab *> _mapKeyTab = std::map<WidgetKey, MainMenuTooltab *>();
	};
} // namespace VTX::UI::Core::MainMenu

#endif
