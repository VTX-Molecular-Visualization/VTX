#include "toolbar.hpp"
#include "model/renderer/render_effect_preset_library.hpp"
#include "model/representation/representation_library.hpp"
#include "ui/widget_factory.hpp"

namespace VTX::UI::Widget::MainMenu::Camera
{
	Toolbar::Toolbar( QWidget * p_parent ) : MenuTooltabWidget( p_parent ) {}
	Toolbar::~Toolbar() {}

	void Toolbar::_setupUi( const QString & p_name )
	{
		MenuTooltabWidget::_setupUi( p_name );

		_cameraAction = addToolBlock<CameraActionBlock>( "cameraActionBlockWidget" );
		_viewpoints	  = addToolBlock<ViewpointBlock>( "viewpointBlockWidget" );

		_renderEffects = WidgetFactory::get().instantiateViewWidget<RenderEffectsBlock>(
			&Model::Renderer::RenderEffectPresetLibrary::get(),
			ID::View::UI_MENU_VISUALIZATION_RENDER_EFFECT_LIBRARY,
			this,
			"renderEffectsBlockWidget" );
		addToolBlock( _renderEffects );

		_background = addToolBlock<BackgroundBlock>( "backgroundBlockWidget" );

		_snapshot = addToolBlock<SnapshotBlock>( "snapshotBlockWidget" );
		_windows  = addToolBlock<WindowsBlock>( "windowsBlockWidget" );
	}
	void Toolbar::_setupSlots() {}
	void Toolbar::localize() {}
} // namespace VTX::UI::Widget::MainMenu::Camera
