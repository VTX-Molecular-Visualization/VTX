#include "ui/old_ui/ui/widget/main_menu/camera/render_effects_block.hpp"
#include "ui/old_ui/style.hpp"
#include "ui/old_ui/ui/dialog.hpp"
#include "ui/old_ui/ui/main_window.hpp"
#include "ui/old_ui/ui/widget/settings/setting_widget_enum.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/old_ui/vtx_app.hpp"
#include <app/core/action/action_manager.hpp>
#include <app/action/main.hpp>
#include <app/old_app/io/filesystem.hpp>
#include <app/old_app/model/renderer/render_effect_preset.hpp>
#include <app/old_app/model/renderer/render_effect_preset_library.hpp>
#include <app/old_app/worker/snapshoter.hpp>

namespace VTX::UI::Widget::MainMenu::Camera
{
	RenderEffectsBlock::RenderEffectsBlock( Model::Renderer::RenderEffectPresetLibrary * const _renderEffectLibrary,
											QWidget *										   p_parent ) :
		View::BaseView<Model::Renderer::RenderEffectPresetLibrary>( _renderEffectLibrary ),
		MenuToolBlockWidget( p_parent ) {};

	void RenderEffectsBlock::receiveEvent( const VTX::Event::VTXEvent & p_event ) {}

	void RenderEffectsBlock::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );
		_instantiateUI();
	}
	void RenderEffectsBlock::_setupSlots()
	{
		_createPreset->setTriggerAction( this, &RenderEffectsBlock::_openPresetSettings );
	}
	void RenderEffectsBlock::localize() { setTitle( "Render Effects" ); }

	void RenderEffectsBlock::notify( const VTX::Event::VTXEvent * const p_event )
	{
		if ( p_event->name == VTX::Event::Model::DISPLAY_NAME_CHANGE
			 || p_event->name == VTX::Event::Model::QUICK_ACCESS_CHANGE
			 || p_event->name == VTX::Event::Model::DATA_CHANGE )
		{
			_refreshView();
		}
	}

	void RenderEffectsBlock::_refreshView()
	{
		reset();

		_instantiateUI();
		_setupSlots();
		localize();
	}

	void RenderEffectsBlock::_instantiateUI()
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

		validate();
	}

	void RenderEffectsBlock::_takeSnapshotAction() const
	{
		VTX_ACTION(
			new Action::Main::Snapshot( Worker::Snapshoter::MODE::GL,
										IO::Filesystem::getUniqueSnapshotsPath( VTX_SETTING().getSnapshotFormat() ),
										VTX_SETTING().getSnapshotResolution() ) );
	}

	void RenderEffectsBlock::_exportAsImageAction() const { Dialog::openAdvancedSettingImageExportDialog(); }

	void RenderEffectsBlock::_openPresetSettings() const
	{
		VTXApp::get().getMainWindow().openSettingWindow( UI::Widget::Settings::SETTING_MENU::RENDER_EFFECTS );
	}

} // namespace VTX::UI::Widget::MainMenu::Camera
