#include "render_preview_widget.hpp"
#include "style.hpp"
#include "worker/snapshoter.hpp"
#include "worker/worker_manager.hpp"

namespace VTX::UI::Widget::CustomWidget
{
	RenderPreviewWidget::RenderPreviewWidget( QWidget * p_parent ) : BaseManualWidget<QLabel>( p_parent ) {}

	void RenderPreviewWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );
		setAttribute( Qt::WA_TranslucentBackground, true );
		setWindowOpacity( 0.5 );
	}
	void RenderPreviewWidget::_setupSlots() {}

	QSize RenderPreviewWidget::sizeHint() const { return _previewPixmpap.size(); }

	void RenderPreviewWidget::takeSnapshot( const IO::Struct::ImageExport & p_exportData )
	{
		QImage preview = QImage();

		IO::Struct::ImageExport	  snapshotData = p_exportData;
		const std::pair<int, int> exportSize   = snapshotData.getSize();
		const float				  exportRatio  = exportSize.first / float( exportSize.second );

		if ( exportRatio >= 1 )
		{
			int snapshotHeight = Style::EXPORT_IMAGE_PREVIEW_SIZE.height() / exportRatio;
			if ( snapshotHeight <= PREVIEW_MIN_SIZE )
				snapshotHeight = PREVIEW_MIN_SIZE;

			snapshotData.setSize( Style::EXPORT_IMAGE_PREVIEW_SIZE.width(), snapshotHeight );
		}
		else
		{
			int snapshotWidth = Style::EXPORT_IMAGE_PREVIEW_SIZE.width() * exportRatio;
			if ( snapshotWidth <= PREVIEW_MIN_SIZE )
				snapshotWidth = PREVIEW_MIN_SIZE;
			snapshotData.setSize( snapshotWidth, Style::EXPORT_IMAGE_PREVIEW_SIZE.height() );
		}

		Worker::Snapshoter * const snapshoter
			= new Worker::Snapshoter( Worker::Snapshoter::MODE::GL, &preview, snapshotData );
		VTX_WORKER( snapshoter );

		const QSize	 imgSize = Style::EXPORT_IMAGE_PREVIEW_SIZE;
		const QImage scaledPreviewImg
			= preview.scaled( imgSize.width(), imgSize.height(), Qt::AspectRatioMode::KeepAspectRatio )
				  .convertToFormat( QImage::Format::Format_ARGB32, Qt::ImageConversionFlag::AutoColor );

		_previewPixmpap = QPixmap::fromImage( scaledPreviewImg );
		setPixmap( _previewPixmpap );

		adjustSize();
	}

	void RenderPreviewWidget::localize() {}

} // namespace VTX::UI::Widget::CustomWidget
