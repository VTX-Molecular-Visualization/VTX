#ifndef __VTX_UI_CORE_MAIN_WINDOW__
#define __VTX_UI_CORE_MAIN_WINDOW__

#include "core/base_panel.hpp"
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
		virtual const BasePanel *		getPanel( const WidgetKey & p_key ) const;

		template<typename T, typename = std::enable_if<std::is_base_of<BasePanel, T>::value>>
		T * getPanel( const WidgetKey & p_key )
		{
			return static_cast<T *>( getPanel( p_key ) );
		}

		template<typename T, typename = std::enable_if<std::is_base_of<BasePanel, T>::value>>
		const T * getPanel( const WidgetKey & p_key ) const
		{
			return static_cast<const T *>( getPanel( p_key ) );
		}

		virtual void referencePanel( const WidgetKey & p_key, BasePanel * const p_panel );

		const std::map<WidgetKey, BasePanel *> & getPanelMap() const { return _getPanelMap(); };
		const WidgetKey &						 findKeyFromPanel( const BasePanel * const p_panel );

	  protected:
		std::map<WidgetKey, BasePanel *> &		 _getPanelMap() { return _mapPanels; };
		const std::map<WidgetKey, BasePanel *> & _getPanelMap() const { return _mapPanels; };

	  private:
		std::map<WidgetKey, BasePanel *> _mapPanels = std::map<WidgetKey, BasePanel *>();
	};

} // namespace VTX::UI::Core

#endif
