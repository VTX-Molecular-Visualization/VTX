#ifndef __VTX_UI_WIDGET_MENU_EXTENSIONS_ALL__
#define __VTX_UI_WIDGET_MENU_EXTENSIONS_ALL__

#ifdef _MSC_VER
#pragma once
#endif

#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/main_menu/extensions/extension_keyword_editor_gui.hpp"
#include "ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include <QDockWidget>
#include <QWidget>

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
