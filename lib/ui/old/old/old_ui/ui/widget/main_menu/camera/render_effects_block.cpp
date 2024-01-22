#include "ui/old_ui/ui/widget/main_menu/camera/render_effects_block.hpp"
#include "ui/old_ui/style.hpp"
#include "ui/old_ui/ui/dialog.hpp"
#include "ui/old_ui/ui/main_window.hpp"
#include "ui/old_ui/ui/widget/settings/setting_widget_enum.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/old_ui/vtx_app.hpp"
#include <app/old/action/main.hpp>
#include <app/old/application/render_effect/render_effect_library.hpp>
#include <app/old/application/render_effect/render_effect_preset.hpp>
#include <app/old/internal/io/filesystem.hpp>
#include <app/old/render/worker/snapshoter.hpp>

namespace VTX::UI::Widget::MainMenu::Camera
{
	RenderEffectsBlock::RenderEffectsBlock(
		App::Old::Application::RenderEffect::RenderEffectLibrary * const _renderEffectLibrary,
		QWidget *													p_parent ) :
		App::Old::Core::View::BaseView<App::Old::Application::RenderEffect::RenderEffectLibrary>( _renderEffectLibrary ),
		MenuToolBlockWidget( p_parent ) {};

	void RenderEffectsBlock::receiveEvent( const VTX::App::Old::Core::Event::VTXEvent & p_event ) {}

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

	void RenderEffectsBlock::notify( const VTX::App::Old::Core::Event::VTXEvent * const p_event )
	{
		if ( p_event->name == VTX::App::Old::Event::Model::DISPLAY_NAME_CHANGE
			 || p_event->name == VTX::App::Old::Event::Model::QUICK_ACCESS_CHANGE
			 || p_event->name == VTX::App::Old::Event::Model::DATA_CHANGE )
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
		for ( int i = 0; i < App::Old::Application::RenderEffect::RenderEffectLibrary::get().getPresetCount(); i++ )
		{
			const App::Old::Application::RenderEffect::RenderEffectPreset * const renderEffectPreset
				= App::Old::Application::RenderEffect::RenderEffectLibrary::get().getPreset( i );

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
		VTX_ACTION( new App::Old::Action::Main::Snapshot(
			App::Old::Render::Worker::Snapshoter::MODE::GL,
			App::Old::Internal::IO::Filesystem::getUniqueSnapshotsPath( VTX_SETTING().getSnapshotFormat() ),
			VTX_SETTING().getSnapshotResolution() ) );
	}

	void RenderEffectsBlock::_exportAsImageAction() const { Dialog::openAdvancedSettingImageExportDialog(); }

	void RenderEffectsBlock::_openPresetSettings() const
	{
		UI::VTXApp::get().getMainWindow().openSettingWindow( UI::Widget::Settings::SETTING_MENU::RENDER_EFFECTS );
	}

} // namespace VTX::UI::Widget::MainMenu::Camera