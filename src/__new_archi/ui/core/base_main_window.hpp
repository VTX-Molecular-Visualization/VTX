#ifndef __VTX_UI_CORE_MAIN_WINDOW__
#define __VTX_UI_CORE_MAIN_WINDOW__

#include "__new_archi/ui/core/base_panel.hpp"
#include "define.hpp"
#include "main_menu/main_menu_bar.hpp"
#include <map>
#include <type_traits>

namespace VTX::UI::Core
{
	enum class WindowMode
	{
		Fullscreen,
		Windowed,
		Maximized,
		Minimized,
	};

	class BaseMainWindow
	{
	  public:
		BaseMainWindow();
		~BaseMainWindow();

		virtual MainMenu::MainMenuBar & getMainMenu() = 0;
		virtual BasePanel *				getPanel( const WidgetKey & p_key );

		template<typename T, typename = std::enable_if<std::is_base_of<BasePanel, T>::value>>
		T * getPanel( const WidgetKey & p_key )
		{
			return dynamic_cast<T *>( getPanel( p_key ) );
		}

		void referencePanel( const WidgetKey & p_key, BasePanel * const p_panel );

	  private:
		std::map<WidgetKey, BasePanel *> _mapPanels = std::map<WidgetKey, BasePanel *>();
	};

} // namespace VTX::UI::Core

#endif
