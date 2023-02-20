#include "menu_camera_widget.hpp"
#include "model/renderer/render_effect_preset_library.hpp"
#include "model/representation/representation_library.hpp"
#include "ui/widget_factory.hpp"

namespace VTX::UI::Widget::MainMenu::Camera
{
	MenuCameraWidget::MenuCameraWidget( QWidget * p_parent ) : MenuTooltabWidget( p_parent ) {}
	MenuCameraWidget::~MenuCameraWidget() {}

	void MenuCameraWidget::_setupUi( const QString & p_name )
	{
		MenuTooltabWidget::_setupUi( p_name );

		_cameraAction = addToolBlock<Visualization::MenuVisualizationCameraActionWidget>( "cameraActionBlockWidget" );
		_viewpoints	  = addToolBlock<Visualization::MenuVisualizationViewpointWidget>( "viewpointBlockWidget" );

		_renderEffects
			= WidgetFactory::get().instantiateViewWidget<Visualization::MenuVisualizationRenderEffectsWidget>(
				&Model::Renderer::RenderEffectPresetLibrary::get(),
				ID::View::UI_MENU_VISUALIZATION_RENDER_EFFECT_LIBRARY,
				this,
				"renderEffectsBlockWidget" );
		addToolBlock( _renderEffects );

		_snapshot = addToolBlock<Visualization::MenuVisualizationSnapshot>( "snapshot" );
		_windows  = addToolBlock<Visualization::MenuVisualizationWindowsWidget>( "windowsBlockWidget" );
	}
	void MenuCameraWidget::_setupSlots() {}
	void MenuCameraWidget::localize() {}
} // namespace VTX::UI::Widget::MainMenu::Camera
