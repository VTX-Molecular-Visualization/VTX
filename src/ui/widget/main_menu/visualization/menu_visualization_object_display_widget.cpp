#include "menu_visualization_object_display_widget.hpp"
#include "ui/widget_factory.hpp"

namespace VTX::UI::Widget::MainMenu::Visualization
{
	MenuVisualizationObjectDisplayWidget::~MenuVisualizationObjectDisplayWidget() {}

	void MenuVisualizationObjectDisplayWidget::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );

		// Selection focus
		_showWater
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "toggleWatersVisibilityButton" );
		_showWater->setData( "Hide Waters", ":/sprite/hide_water_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_showWater, 0 );

		_showSolvant
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "toggleSolvantVisibilityButton" );
		_showSolvant->setData( "Hide Solvents", ":/sprite/hide_solvant_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_showSolvant, 0 );

		_showHydrogens
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "toggleHydrogensVisibilityButton" );
		_showHydrogens->setData( "Hide Hydrogens", ":/sprite/hide_hydrogen_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_showHydrogens, 0 );

		validate();
	}
	void MenuVisualizationObjectDisplayWidget::_setupSlots()
	{
		connect( _showWater,
				 &MenuToolButtonWidget::clicked,
				 this,
				 &MenuVisualizationObjectDisplayWidget::_toggleWaterVisibilityAction );

		connect( _showSolvant,
				 &MenuToolButtonWidget::clicked,
				 this,
				 &MenuVisualizationObjectDisplayWidget::_toggleSolventVisibilityAction );

		connect( _showHydrogens,
				 &MenuToolButtonWidget::clicked,
				 this,
				 &MenuVisualizationObjectDisplayWidget::_toggleHydrogenVisibilityAction );
	}
	void MenuVisualizationObjectDisplayWidget::localize() { setTitle( "Object Display" ); }

	void MenuVisualizationObjectDisplayWidget::_toggleWaterVisibilityAction() const {}
	void MenuVisualizationObjectDisplayWidget::_toggleSolventVisibilityAction() const {}
	void MenuVisualizationObjectDisplayWidget::_toggleHydrogenVisibilityAction() const {}

} // namespace VTX::UI::Widget::MainMenu::Visualization
