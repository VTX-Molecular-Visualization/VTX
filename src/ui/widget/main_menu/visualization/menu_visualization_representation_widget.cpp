#include "menu_visualization_representation_widget.hpp"
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
					MenuVisualizationRepresentationWidget::MenuVisualizationRepresentationWidget( QWidget * p_parent ) : MenuToolBlockWidget( p_parent ) {}
					MenuVisualizationRepresentationWidget::~MenuVisualizationRepresentationWidget() {}

					void MenuVisualizationRepresentationWidget::_setupUi( const QString & p_name )
					{
						MenuToolBlockWidget::_setupUi( p_name );

						_preset1 = WidgetFactory::get().getWidget<RepresentationPresetButton>( this, "applyVisualModelPresetButton" );
						_preset1->setData( "Sticks", ":/sprite/representation_stick_icon.png", Qt::Orientation::Horizontal );
						_preset1->setRepresentationID( 0 );
						pushButton( *_preset1, 0 );

						_preset2 = WidgetFactory::get().getWidget<RepresentationPresetButton>( this, "applyVisualModelPresetButton" );
						_preset2->setData( "Trace", ":/sprite/representation_trace_icon.png", Qt::Orientation::Horizontal );
						_preset2->setRepresentationID( 1 );
						pushButton( *_preset2, 0 );

						_preset3 = WidgetFactory::get().getWidget<RepresentationPresetButton>( this, "applyVisualModelPresetButton" );
						_preset3->setData( "SAS", ":/sprite/representation_sas_icon.png", Qt::Orientation::Horizontal );
						_preset3->setRepresentationID( 2 );
						pushButton( *_preset3, 0 );

						_preset4 = WidgetFactory::get().getWidget<RepresentationPresetButton>( this, "applyVisualModelPresetButton" );
						_preset4->setData( "Custom preset 1", "", Qt::Orientation::Horizontal );
						_preset4->setRepresentationID( 3 );
						pushButton( *_preset4, 1 );

						_preset5 = WidgetFactory::get().getWidget<RepresentationPresetButton>( this, "applyVisualModelPresetButton" );
						_preset5->setData( "Custom preset 2", "", Qt::Orientation::Horizontal );
						_preset5->setRepresentationID( 4 );
						pushButton( *_preset5, 1 );

						_preset6 = WidgetFactory::get().getWidget<RepresentationPresetButton>( this, "applyVisualModelPresetButton" );
						_preset6->setData( "Custom preset 3", "", Qt::Orientation::Horizontal );
						_preset6->setRepresentationID( 5 );
						pushButton( *_preset6, 1 );

						_addPreset = WidgetFactory::get().getWidget<RepresentationPresetButton>( this, "addNewVisualModelPresetButton" );
						_addPreset->setData( "Add\nPreset", ":/sprite/representation_new_preset_icon.png", Qt::Orientation::Vertical );
						pushButton( *_addPreset, 2 );

						validate();
					}
					void MenuVisualizationRepresentationWidget::_setupSlots() {}

					void MenuVisualizationRepresentationWidget::localize() { setTitle( "Representation" ); }
				} // namespace Visualization
			}	  // namespace MainMenu
		}		  // namespace Widget
	}			  // namespace UI
} // namespace VTX
