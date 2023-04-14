#include "menu_extensions_all_widget.hpp"
#include <app/old_app/action/main.hpp>
#include <app/old_app/io/filesystem.hpp>
#include <ui/old_ui/ui/widget_factory.hpp>

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
					MenuExtensionsAllWidgets::~MenuExtensionsAllWidgets() {}

					void MenuExtensionsAllWidgets::_setupUi( const QString & p_name )
					{
						MenuToolBlockWidget::_setupUi( p_name );

						// Add extensions buttons
						_keywordEditor = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>(
							this, "keywordEditorButton" );
						_keywordEditor->setData(
							"Keyword\nEditor", ":/sprite/keyword_editor.png", Qt::Orientation::Vertical );
						pushButton( *_keywordEditor, 0 );

						validate();
					}
					void MenuExtensionsAllWidgets::_setupSlots()
					{
						_keywordEditor->setTriggerAction( this,
														  &MenuExtensionsAllWidgets::_showKeywordEditorWindowAction );
					}
					void MenuExtensionsAllWidgets::localize() { setTitle( "All Modules" ); }

					void MenuExtensionsAllWidgets::_showKeywordEditorWindowAction()
					{
						if ( _keywordEditorWindow == nullptr )
						{
							_keywordEditorWindow = new ExtensionKeywordEditorGUI( this );
							_keywordEditorWindow->show();
						}
						else
						{
							if ( !_keywordEditorWindow->isVisible() )
							{
								_keywordEditorWindow->show();
							}
							else
							{
								_keywordEditorWindow->raise();
							}
						}
					}
				} // namespace Extensions
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
