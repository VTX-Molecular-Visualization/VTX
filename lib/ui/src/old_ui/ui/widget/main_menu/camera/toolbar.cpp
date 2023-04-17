#include "ui/old_ui/ui/widget/main_menu/camera/toolbar.hpp"
#include "ui/old_ui/ui/widget/main_menu/camera/background_block.hpp"
#include "ui/old_ui/ui/widget/main_menu/camera/camera_navigation_action_block.hpp"
#include "ui/old_ui/ui/widget/main_menu/camera/camera_projection_block.hpp"
#include "ui/old_ui/ui/widget/main_menu/camera/render_effects_block.hpp"
#include "ui/old_ui/ui/widget/main_menu/camera/snapshot_block.hpp"
#include "ui/old_ui/ui/widget/main_menu/camera/viewpoint_block.hpp"
#include "ui/old_ui/ui/widget/main_menu/camera/windows_block.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include <app/old_app/model/renderer/render_effect_preset_library.hpp>
#include <app/old_app/model/representation/representation_library.hpp>

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
