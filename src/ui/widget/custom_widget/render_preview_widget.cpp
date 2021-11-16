#include "render_preview_widget.hpp"
#include "style.hpp"
#include "worker/snapshoter.hpp"
#include "worker/worker_manager.hpp"

namespace VTX::UI::Widget::CustomWidget
{
	RenderPreviewWidget::RenderPreviewWidget( QWidget * p_parent ) : BaseManualWidget<QLabel>( p_parent ) {}

	void RenderPreviewWidget::_setupUi( const QString & p_name ) { BaseManualWidget::_setupUi( p_name ); }
	void RenderPreviewWidget::_setupSlots() {}

	QSize RenderPreviewWidget::sizeHint() const { return _previewPixmpap.size(); }

	void RenderPreviewWidget::takeSnapshot( const IO::Struct::ImageExport & p_exportData )
	{
		QImage preview = QImage();

		IO::Struct::ImageExport	  snapshotData = p_exportData;
		const std::pair<int, int> exportSize   = snapshotData.getSize();
		const float				  exportRatio  = exportSize.first / float( exportSize.second );

		snapshotData.setSize( Style::EXPORT_IMAGE_PREVIEW_SIZE.width() * exportRatio,
							  Style::EXPORT_IMAGE_PREVIEW_SIZE.height() );

		Worker::Snapshoter * const snapshoter
			= new Worker::Snapshoter( Worker::Snapshoter::MODE::GL, &preview, snapshotData );
		VTX_WORKER( snapshoter );

		const QSize	 imgSize = Style::EXPORT_IMAGE_PREVIEW_SIZE;
		const QImage scaledPreviewImg
			= preview.scaled( imgSize.width(), imgSize.height(), Qt::AspectRatioMode::KeepAspectRatio );

		_previewPixmpap = QPixmap::fromImage( scaledPreviewImg );
		setPixmap( _previewPixmpap );

		adjustSize();
	}

	void RenderPreviewWidget::localize() {}

} // namespace VTX::UI::Widget::CustomWidget
