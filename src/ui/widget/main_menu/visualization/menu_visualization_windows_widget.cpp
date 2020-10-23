#include "menu_visualization_windows_widget.hpp"
#include "ui/widget_factory.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace MainMenu
			{
				namespace Visualization
				{
					MenuVisualizationWindowsWidget::~MenuVisualizationWindowsWidget() {}

					void MenuVisualizationWindowsWidget::_setupUi( const QString & p_name )
					{
						MenuToolBlockWidget::_setupUi( p_name );

						// Render view tools
						_minimap = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "showMinimapButton" );
						_minimap->setData( " Show Minimap", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_minimap, 0 );

						_infoUnderCursor = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "showInfoUnderCursorButton" );
						_infoUnderCursor->setData( " Show Cursor Info", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_infoUnderCursor, 0 );

						_sequence = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "toggleSequenceButton" );
						_sequence->setData( " Toggle Sequence", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_sequence, 0 );

						validate();
					}
					void MenuVisualizationWindowsWidget::_setupSlots() { _sequence->setTriggerAction( this, &MenuVisualizationWindowsWidget::_openSequenceWindow ); }
					void MenuVisualizationWindowsWidget::localize() { setTitle( "Windows" ); }

					void MenuVisualizationWindowsWidget::_openSequenceWindow() { VTXApp::get().getMainWindow().toggleSequenceWindow(); }
				} // namespace Visualization
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
