#include "menu_visualization_object_display_widget.hpp"
#include "ui/widget_factory.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			MenuVisualizationObjectDisplayWidget::~MenuVisualizationObjectDisplayWidget() {}

			void MenuVisualizationObjectDisplayWidget::_setupUi( const QString & p_name )
			{
				MenuToolBlockWidget::_setupUi( p_name );

				// Selection focus
				_showWater = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "toggleWatersVisibilityButton" );
				_showWater->setData( "Hide Waters", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
				pushButton( *_showWater, 0 );

				_showSolvant = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "toggleSolvantVisibilityButton" );
				_showSolvant->setData( "Hide Solvant", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
				pushButton( *_showSolvant, 0 );

				_showHydrogens = WidgetFactory::get().GetWidget<MenuToolButtonWidget>( this, "toggleHydrogensVisibilityButton" );
				_showHydrogens->setData( "Hide Hydrogen", ":/sprite/new_session_icon.png", Qt::Orientation::Horizontal );
				pushButton( *_showHydrogens, 0 );

				validate();
			}
			void MenuVisualizationObjectDisplayWidget::_setupSlots() {}
			void MenuVisualizationObjectDisplayWidget::localize() { setTitle( "Object Display" ); }
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
