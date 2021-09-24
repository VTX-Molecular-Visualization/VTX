#include "menu_visualization_render_effects_widget.hpp"
#include "action/main.hpp"
#include "model/renderer/render_effect_preset.hpp"
#include "model/renderer/render_effect_preset_library.hpp"
#include "style.hpp"
#include "ui/main_window.hpp"
#include "ui/widget/settings/setting_widget_enum.hpp"
#include "ui/widget_factory.hpp"
#include "util/filesystem.hpp"
#include "vtx_app.hpp"
#include "worker/snapshoter.hpp"

namespace VTX::UI::Widget::MainMenu::Visualization
{
	MenuVisualizationRenderEffectsWidget::MenuVisualizationRenderEffectsWidget(
		Model::Renderer::RenderEffectPresetLibrary * const _renderEffectLibrary,
		QWidget *										   p_parent ) :
		View::BaseView<Model::Renderer::RenderEffectPresetLibrary>( _renderEffectLibrary ),
		MenuToolBlockWidget( p_parent )
	{
		_registerEvent( Event::Global::MAIN_WINDOW_MODE_CHANGE );
	};

	void MenuVisualizationRenderEffectsWidget::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::MAIN_WINDOW_MODE_CHANGE )
		{
			const WindowMode mode = dynamic_cast<const Event::VTXEventValue<WindowMode> &>( p_event ).value;
			_updateFullscreenButton( mode );
		}
	}

	void MenuVisualizationRenderEffectsWidget::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );
		_instantiateUI();
	}
	void MenuVisualizationRenderEffectsWidget::_setupSlots()
	{
		_createPreset->setTriggerAction( this, &MenuVisualizationRenderEffectsWidget::_openPresetSettings );
		_takeSnapshot->setTriggerAction( this, &MenuVisualizationRenderEffectsWidget::_takeSnapshotAction );
		_fullscreen->setTriggerAction( this, &MenuVisualizationRenderEffectsWidget::_toggleWindowState );
	}
	void MenuVisualizationRenderEffectsWidget::localize() { setTitle( "Render Effects" ); }

	void MenuVisualizationRenderEffectsWidget::notify( const Event::VTXEvent * const p_event )
	{
		if ( p_event->name == Event::Model::DISPLAY_NAME_CHANGE || p_event->name == Event::Model::QUICK_ACCESS_CHANGE
			 || p_event->name == Event::Model::DATA_CHANGE )
		{
			_refreshView();
		}
	}

	void MenuVisualizationRenderEffectsWidget::_refreshView()
	{
		reset();

		_instantiateUI();
		_setupSlots();
		localize();
	}

	void MenuVisualizationRenderEffectsWidget::_instantiateUI()
	{
		_presetButtons.clear();

		int quickAccessRepresentationCount = 0;
		for ( int i = 0; i < Model::Renderer::RenderEffectPresetLibrary::get().getPresetCount(); i++ )
		{
			const Model::Renderer::RenderEffectPreset * const renderEffectPreset
				= Model::Renderer::RenderEffectPresetLibrary::get().getPreset( i );

			if ( !renderEffectPreset->hasQuickAccess() )
				continue;

			RenderEffectPresetButton * const button = WidgetFactory::get().instantiateWidget<RenderEffectPresetButton>(
				this, "applyRenderEffectPresetButton" );

			button->setRenderEffectID( i );
			button->setData( QString::fromStdString( renderEffectPreset->getName() ),
							 ":/sprite/render_effect_preset_icon.png",
							 Qt::Orientation::Horizontal );
			pushButton( *button, quickAccessRepresentationCount / MAX_ROW_COUNT );
			_presetButtons.emplace_back( button );

			quickAccessRepresentationCount++;
		}

		for ( int i = 0; i < getColumnCount(); i++ )
			setRowCountInColumn( i, MAX_ROW_COUNT );

		// Add Preset
		_createPreset
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "createRenderPresetButton" );
		_createPreset->setData(
			"Preset\nSettings", ":/sprite/render_effect_settings_icon.png", Qt::Orientation::Vertical );
		pushButtonInNextColumn( *_createPreset );

		// Fullscreen / snapshot
		_fullscreen = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "toggleFullscreenButton" );
		_fullscreen->setData( "Fullscreen", ":/sprite/fullscreen_icon.png", Qt::Orientation::Horizontal );
		_updateFullscreenButton( VTXApp::get().getMainWindow().getWindowMode() );
		const int lastColumn = pushButtonInNextColumn( *_fullscreen );

		_takeSnapshot = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "takeSnapshotButton" );
		_takeSnapshot->setData( "Snapshot", ":/sprite/screenshot_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_takeSnapshot, lastColumn );

		validate();
	}

	void MenuVisualizationRenderEffectsWidget::_updateFullscreenButton( const WindowMode & p_mode )
	{
		if ( p_mode == WindowMode::Fullscreen )
		{
			_fullscreen->setIcon( Style::IconConst::get().WINDOWED_ICON );
			_fullscreen->setText( "Window" );
		}
		else
		{
			_fullscreen->setIcon( Style::IconConst::get().FULLSCREEN_ICON );
			_fullscreen->setText( "Fullscreen" );
		}
	}

	void MenuVisualizationRenderEffectsWidget::_takeSnapshotAction() const
	{
		VTX_ACTION(
			new Action::Main::Snapshot( Worker::Snapshoter::MODE::GL,
										Util::Filesystem::getSnapshotsPath( Util::Time::getTimestamp() + ".png" ),
										VTX_SETTING().getSnapshotResolution() ) );
	}
	void MenuVisualizationRenderEffectsWidget::_openPresetSettings() const
	{
		VTXApp::get().getMainWindow().openSettingWindow( UI::Widget::Settings::SETTING_MENU::RENDER_EFFECTS );
	}
	void MenuVisualizationRenderEffectsWidget::_toggleWindowState() const
	{
		const Qt::WindowStates windowState = VTXApp::get().getMainWindow().windowState();

		if ( windowState & Qt::WindowStates::enum_type::WindowFullScreen )
			VTX_ACTION( new Action::Setting::WindowMode( WindowMode::Windowed ) );
		else
			VTX_ACTION( new Action::Setting::WindowMode( WindowMode::Fullscreen ) );
	}

} // namespace VTX::UI::Widget::MainMenu::Visualization
