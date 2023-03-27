#include "toolbar.hpp"
#include "background_block.hpp"
#include "camera_navigation_action_block.hpp"
#include "camera_projection_block.hpp"
#include "model/renderer/render_effect_preset_library.hpp"
#include "model/representation/representation_library.hpp"
#include "render_effects_block.hpp"
#include "snapshot_block.hpp"
#include "ui/widget_factory.hpp"
#include "viewpoint_block.hpp"
#include "windows_block.hpp"

namespace VTX::UI::Widget::MainMenu::Camera
{
	Toolbar::Toolbar( QWidget * p_parent ) : MenuTooltabWidget( p_parent ) {}
	Toolbar::~Toolbar() {}

	void Toolbar::_setupUi( const QString & p_name )
	{
		MenuTooltabWidget::_setupUi( p_name );

		addToolBlock<CameraProjectionBlock>( "cameraProjectionBlockWidget" );
		addToolBlock<CameraNavigationActionBlock>( "cameraNavigationBlockWidget" );
		addToolBlock<ViewpointBlock>( "viewpointBlockWidget" );
		addToolBlock<BackgroundBlock>( "backgroundBlockWidget" );

		RenderEffectsBlock * const renderEffects = WidgetFactory::get().instantiateViewWidget<RenderEffectsBlock>(
			&Model::Renderer::RenderEffectPresetLibrary::get(),
			ID::View::UI_MENU_VISUALIZATION_RENDER_EFFECT_LIBRARY,
			this,
			"renderEffectsBlockWidget" );
		addToolBlock( renderEffects );

		addToolBlock<SnapshotBlock>( "snapshotBlockWidget" );
		addToolBlock<WindowsBlock>( "windowsBlockWidget" );
	}
	void Toolbar::_setupSlots() {}
	void Toolbar::localize() {}
} // namespace VTX::UI::Widget::MainMenu::Camera
