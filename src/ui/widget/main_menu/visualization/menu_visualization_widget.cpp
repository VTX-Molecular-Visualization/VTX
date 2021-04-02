#include "menu_visualization_widget.hpp"
#include "model/representation/representation_library.hpp"
#include "ui/widget_factory.hpp"

namespace VTX::UI::Widget::MainMenu::Visualization
{
	MenuVisualizationWidget::MenuVisualizationWidget( QWidget * p_parent ) : MenuTooltabWidget( p_parent ) {}
	MenuVisualizationWidget::~MenuVisualizationWidget() {}

	void MenuVisualizationWidget::_setupUi( const QString & p_name )
	{
		MenuTooltabWidget::_setupUi( p_name );

		_cameraAction = addToolBlock<MenuVisualizationCameraActionWidget>( "cameraActionBlockWidget" );

		_representation = WidgetFactory::get().instantiateViewWidget<MenuVisualizationRepresentationWidget>(
			&Model::Representation::RepresentationLibrary::get(),
			ID::View::UI_MENU_VISUALIZATION_REPRESENTATION_LIBRARY,
			this,
			"RepresentationBlockWidget" );
		addToolBlock( _representation );

		_objectDisplay	 = addToolBlock<MenuVisualizationObjectDisplayWidget>( "objectDisplayBlockWidget" );
		_renderEffects	 = addToolBlock<MenuVisualizationRenderEffectsWidget>( "renderEffectsBlockWidget" );
		_selectionAction = addToolBlock<MenuVisualizationSelectionActionWidget>( "selectionBlockWidget" );
		_windows		 = addToolBlock<MenuVisualizationWindowsWidget>( "windowsBlockWidget" );
	}
	void MenuVisualizationWidget::_setupSlots() {}
	void MenuVisualizationWidget::localize() {}
} // namespace VTX::UI::Widget::MainMenu::Visualization
