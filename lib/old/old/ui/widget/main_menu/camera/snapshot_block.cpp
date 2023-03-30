#include "snapshot_block.hpp"
#include "action/action_manager.hpp"
#include "action/main.hpp"
#include "model/renderer/render_effect_preset.hpp"
#include "model/renderer/render_effect_preset_library.hpp"
#include "style.hpp"
#include "ui/dialog.hpp"
#include "ui/main_window.hpp"
#include "ui/widget/settings/setting_widget_enum.hpp"
#include "ui/widget_factory.hpp"
#include <util/filesystem.hpp>
#include "vtx_app.hpp"
#include "worker/snapshoter.hpp"

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
		VTX_ACTION( new Action::Main::Snapshot( Worker::Snapshoter::MODE::GL,
												Util::Filesystem::getUniqueSnapshotsPath(),
												VTX_SETTING().getSnapshotResolution() ) );
	}

	void SnapshotBlock::_exportAsImageAction() const { Dialog::openAdvancedSettingImageExportDialog(); }
} // namespace VTX::UI::Widget::MainMenu::Camera
