#include "menu_visualization_selection_action_widget.hpp"
#include "ui/widget_factory.hpp"

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
					MenuVisualizationSelectionActionWidget::~MenuVisualizationSelectionActionWidget() {}

					void MenuVisualizationSelectionActionWidget::_setupUi( const QString & p_name )
					{
						MenuToolBlockWidget::_setupUi( p_name );

						// Selection actions 1
						_copy = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "copySelectionButton" );
						_copy->setData( "Copy", ":/sprite/copy_selection_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_copy, 0 );

						_extract = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "extractSelectionButton" );
						_extract->setData( "Extract", ":/sprite/extract_selection_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_extract, 0 );

						_delete = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "deleteSelectionButton" );
						_delete->setData( "Delete", ":/sprite/delete_selection_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_delete, 0 );

						// Visibility actions
						_show = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "showSelectionButton" );
						_show->setData( "Show", ":/sprite/show_selection_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_show, 1 );

						_hide = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "hideSelectionButton" );
						_hide->setData( "Hide", ":/sprite/hide_selection_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_hide, 1 );

						validate();
					}
					void MenuVisualizationSelectionActionWidget::_setupSlots() {}
					void MenuVisualizationSelectionActionWidget::localize() { setTitle( "Selection Action" ); }
				} // namespace Visualization
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
