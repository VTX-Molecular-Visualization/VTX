#include "ui/old_ui/ui/widget/main_menu/camera/snapshot_block.hpp"
#include "ui/old_ui/style.hpp"
#include "ui/old_ui/ui/dialog.hpp"
#include "ui/old_ui/ui/main_window.hpp"
#include "ui/old_ui/ui/widget/settings/setting_widget_enum.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/old_ui/vtx_app.hpp"

#include <app/action/main.hpp>
#include <app/old_app/io/filesystem.hpp>
#include <app/model/renderer/render_effect_preset.hpp>
#include <app/model/renderer/render_effect_preset_library.hpp>
#include <app/worker/snapshoter.hpp>

namespace VTX::UI::Widget::MainMenu::Camera
{
	SnapshotBlock::SnapshotBlock( QWidget * p_parent ) : MenuToolBlockWidget( p_parent ) {};

	void SnapshotBlock::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );

		_takeSnapshot = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "takeSnapshotButton" );
		_takeSnapshot->setData( "Snapshot", ":/sprite/screenshot_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_takeSnapshot, 0 );

		_exportAsImage = WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "exportAsImageButton" );
		_exportAsImage->setData( "Export\nImage...", ":/sprite/screenshot_icon.png", Qt::Orientation::Horizontal );
		pushButton( *_exportAsImage, 0 );

		validate();
	}
	void SnapshotBlock::_setupSlots()
	{
		_takeSnapshot->setTriggerAction( this, &SnapshotBlock::_takeSnapshotAction );
		_exportAsImage->setTriggerAction( this, &SnapshotBlock::_exportAsImageAction );
	}
	void SnapshotBlock::localize() { setTitle( "Snapshot" ); }

	void SnapshotBlock::_takeSnapshotAction() const
	{
		VTX_ACTION(
			new App::Action::Main::Snapshot( Worker::Snapshoter::MODE::GL,
										IO::Filesystem::getUniqueSnapshotsPath( VTX_SETTING().getSnapshotFormat() ),
										VTX_SETTING().getSnapshotResolution() ) );
	}

	void SnapshotBlock::_exportAsImageAction() const { Dialog::openAdvancedSettingImageExportDialog(); }
} // namespace VTX::UI::Widget::MainMenu::Camera
