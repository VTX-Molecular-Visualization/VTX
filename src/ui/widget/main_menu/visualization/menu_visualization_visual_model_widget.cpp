#include "menu_visualization_visual_model_widget.hpp"
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
					MenuVisualizationVisualModelWidget::MenuVisualizationVisualModelWidget( QWidget * p_parent ) : MenuToolBlockWidget( p_parent ) {}
					MenuVisualizationVisualModelWidget::~MenuVisualizationVisualModelWidget() {}

					void MenuVisualizationVisualModelWidget::_setupUi( const QString & p_name )
					{
						MenuToolBlockWidget::_setupUi( p_name );

						_preset1 = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "applyVisualModelPresetButton" );
						_preset1->setData( "Sticks", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_preset1, 0 );

						_preset2 = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "applyVisualModelPresetButton" );
						_preset2->setData( "Trace", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_preset2, 0 );

						_preset3 = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "applyVisualModelPresetButton" );
						_preset3->setData( "SAS", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_preset3, 0 );

						_preset4 = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "applyVisualModelPresetButton" );
						_preset4->setData( "Custom preset 1", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_preset4, 1 );

						_preset5 = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "applyVisualModelPresetButton" );
						_preset5->setData( "Custom preset 2", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_preset5, 1 );

						_preset6 = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "applyVisualModelPresetButton" );
						_preset6->setData( "Custom preset 3", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
						pushButton( *_preset6, 1 );

						_addPreset = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "addNewVisualModelPresetButton" );
						_addPreset->setData( "Add Preset", ":/sprite/new_session_icon.png", Qt::Orientation::Vertical );
						pushButton( *_addPreset, 2 );

						validate();
					}
					void MenuVisualizationVisualModelWidget::_setupSlots() {}

					void MenuVisualizationVisualModelWidget::localize() { setTitle( "Visual Model" ); }
				} // namespace Visualization
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
