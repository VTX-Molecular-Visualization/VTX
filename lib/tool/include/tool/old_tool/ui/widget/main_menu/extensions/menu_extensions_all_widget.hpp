#ifndef __VTX_UI_WIDGET_MENU_EXTENSIONS_ALL__
#define __VTX_UI_WIDGET_MENU_EXTENSIONS_ALL__

#include "tool/old_tool/ui/widget/main_menu/extensions/extension_keyword_editor_gui.hpp"
#include <QDockWidget>
#include <QWidget>
#include <ui/old_ui/ui/widget/base_manual_widget.hpp>
#include <ui/old_ui/ui/widget/main_menu/menu_toolblock_widget.hpp>
#include <ui/old_ui/ui/widget/main_menu/menu_toolbutton_widget.hpp>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace MainMenu
			{
				namespace Extensions
				{
					class MenuExtensionsAllWidgets : public MenuToolBlockWidget
					{
						VTX_WIDGET

					  public:
						~MenuExtensionsAllWidgets();
						void localize() override;

					  protected:
						MenuExtensionsAllWidgets( QWidget * p_parent ) : MenuToolBlockWidget( p_parent ) {};
						void _setupUi( const QString & p_name ) override;
						void _setupSlots() override;

					  private:
						// Widgets list
						MenuToolButtonWidget * _keywordEditor = nullptr;

						// Widgets windows
						ExtensionKeywordEditorGUI * _keywordEditorWindow = nullptr;

						// ACTIONS //
						void _showKeywordEditorWindowAction();
					};
				} // namespace Extensions
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
#endif
