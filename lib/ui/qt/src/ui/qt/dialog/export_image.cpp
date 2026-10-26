#include "ui/qt/dialog/export_image.hpp"
#include "app/action/io.hpp"
#include "app/services.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/services.hpp"
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QImage>
#include <QPixmap>
#include <QSignalBlocker>
#include <QSizePolicy>
#include <QTimer>
#include <QVBoxLayout>
#include <array>
#include <memory>
#include <renderer/renderer.hpp>
#include <string_view>
#include <util/image.hpp>
#include <util/resolution.hpp>
#include <utility>
#include <vector>

namespace
{
	constexpr double RATIO_MIN				 = 0.05;
	constexpr double RATIO_MAX				 = 5.0;
	constexpr int	 PREVIEW_FALLBACK_WIDTH	 = 460;
	constexpr int	 PREVIEW_FALLBACK_HEIGHT = 220;
	constexpr int	 PREVIEW_DEBOUNCE_MS	 = 200;

	constexpr auto FORMATS = std::to_array<std::string_view>( { "PNG - Portable Network Graphics",
																"JPEG - Joint Photographic Experts Group" } );

	constexpr VTX::Util::Resolution CURRENT_RESOLUTION_PRESET = { "Current", 0, 0 };

	constexpr auto IMAGE_EXPORT_RESOLUTION_PRESETS = std::to_array<VTX::Util::Resolution>( {
		CURRENT_RESOLUTION_PRESET,
		VTX::Util::ResolutionPreset::SVGA,
		VTX::Util::ResolutionPreset::XGA,
		VTX::Util::ResolutionPreset::HD,
		VTX::Util::ResolutionPreset::WXGA,
		VTX::Util::ResolutionPreset::SXGA,
		VTX::Util::ResolutionPreset::WXGA_PLUS,
		VTX::Util::ResolutionPreset::HD_PLUS,
		VTX::Util::ResolutionPreset::WSXGA_PLUS,
		VTX::Util::ResolutionPreset::FULL_HD,
		VTX::Util::ResolutionPreset::WUXGA,
		VTX::Util::ResolutionPreset::QHD,
		VTX::Util::ResolutionPreset::WQXGA,
		VTX::Util::ResolutionPreset::UHD_4K,
		VTX::Util::ResolutionPreset::UHD_8K,
		VTX::Util::ResolutionPreset::UHD_16K,
	} );
} // namespace

namespace VTX::UI::QT::Dialog
{

	ExportImage::ExportImage()
	{
		setWindowTitle( "Export image" );
		setFixedSize( 500, 540 );

		auto * layout = Helper::makeQObject<QVBoxLayout>( this );

		// Resolution.
		auto * groupResolution		 = new QGroupBox( "Resolution" );
		auto * layoutGroupResolution = Helper::makeQObject<QVBoxLayout>( groupResolution );
		groupResolution->setLayout( layoutGroupResolution );
		groupResolution->setSizePolicy( QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Maximum );
		auto * layoutPresetSize = Helper::makeChildLayout<QHBoxLayout>( *layoutGroupResolution );

		layoutPresetSize->setAlignment( Qt::AlignmentFlag::AlignLeft );

		auto * labelResolution = Helper::makeQObject<QLabel>( "Preset", this );
		_comboBoxResolution	   = Helper::makeQObject<QComboBox>( this );
		_comboBoxResolution->addItem( "-select-" );
		_comboBoxResolution->setInsertPolicy( QComboBox::InsertPolicy::NoInsert );

		auto   resolutionPresets = IMAGE_EXPORT_RESOLUTION_PRESETS;
		auto & currentResolution = resolutionPresets[ 0 ];
		currentResolution.width	 = App::RENDERER().width();
		currentResolution.height = App::RENDERER().height();

		const int maxTextureSize = static_cast<int>( App::RENDERER().getInfos().maxTextureSize );

		for ( const auto & resolution : resolutionPresets )
		{
			if ( std::cmp_greater( resolution.width, maxTextureSize )
				 || std::cmp_greater( resolution.height, maxTextureSize ) )
			{
				continue;
			}

			QString text = QString::fromUtf8( resolution.name );
			text += QString( " (%1x%2)" ).arg( resolution.width ).arg( resolution.height );
			_comboBoxResolution->addItem( text );
			const int itemIndex = _comboBoxResolution->count() - 1;
			_comboBoxResolution->setItemData( itemIndex, static_cast<int>( resolution.width ), Qt::UserRole );
			_comboBoxResolution->setItemData( itemIndex, static_cast<int>( resolution.height ), Qt::UserRole + 1 );
		}

		connect( _comboBoxResolution, QOverload<int>::of( &QComboBox::activated ), this, &ExportImage::_onResolution );

		layoutPresetSize->addWidget( labelResolution );
		layoutPresetSize->addWidget( _comboBoxResolution );

		// Size widgets.
		auto * labelWidth = Helper::makeQObject<QLabel>( "Width", this );
		_spinBoxWidth	  = Helper::makeQObject<QSpinBox>( this );
		_spinBoxWidth->setMinimum( 100 );
		_spinBoxWidth->setMaximum( maxTextureSize );
		_spinBoxWidth->setSizePolicy( QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed );

		auto * labelHeight = Helper::makeQObject<QLabel>( "Height", this );
		_spinBoxHeight	   = Helper::makeQObject<QSpinBox>( this );
		_spinBoxHeight->setMinimum( 100 );
		_spinBoxHeight->setMaximum( maxTextureSize );
		_spinBoxHeight->setSizePolicy( QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed );

		connect( _spinBoxWidth, QOverload<int>::of( &QSpinBox::valueChanged ), this, &ExportImage::_onSize );
		connect( _spinBoxHeight, QOverload<int>::of( &QSpinBox::valueChanged ), this, &ExportImage::_onSize );

		layoutPresetSize->addWidget( labelWidth );
		layoutPresetSize->addWidget( _spinBoxWidth );
		layoutPresetSize->addWidget( labelHeight );
		layoutPresetSize->addWidget( _spinBoxHeight );

		// Ratio widgets.
		auto * layoutRatio = Helper::makeChildLayout<QHBoxLayout>( *layoutGroupResolution );
		layoutRatio->setAlignment( Qt::AlignmentFlag::AlignCenter );

		auto * labelRatio = Helper::makeQObject<QLabel>( "Ratio", this );
		labelRatio->setToolTip( "Adjust height from ratio (ratio = width / height)" );

		_labelRatioValue = Helper::makeQObject<QLabel>( this );

		_sliderRatio = Helper::makeQObject<QSlider>( Qt::Orientation::Horizontal, this );
		_sliderRatio->setMinimum( static_cast<int>( RATIO_MIN * 10000 ) );
		_sliderRatio->setMaximum( static_cast<int>( RATIO_MAX * 10000 ) );

		connect( _sliderRatio, &QSlider::valueChanged, this, &ExportImage::_onRatio );

		layoutRatio->addWidget( labelRatio );
		layoutRatio->addWidget( _sliderRatio );
		layoutRatio->addWidget( _labelRatioValue );

		// Format.
		auto * groupFormat	= new QGroupBox( "Format" );
		auto * layoutFormat = Helper::makeQObject<QVBoxLayout>( groupFormat );
		groupFormat->setLayout( layoutFormat );
		groupFormat->setSizePolicy( QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Maximum );

		_comboBoxFormat = Helper::makeQObject<QComboBox>( this );
		_comboBoxFormat->addItem( QString::fromUtf8( FORMATS[ 0 ] ) );
		_comboBoxFormat->addItem( QString::fromUtf8( FORMATS[ 1 ] ) );

		connect(
			_comboBoxFormat, QOverload<int>::of( &QComboBox::currentIndexChanged ), this, &ExportImage::_onFormat
		);

		layoutFormat->addWidget( _comboBoxFormat );

		// Background opacity.
		auto * layoutBackgroundOpacity = Helper::makeChildLayout<QHBoxLayout>( *layoutFormat );
		_labelBackgroundOpacity		   = Helper::makeQObject<QLabel>( "Background opacity", this );
		_sliderBackgroundOpacity	   = Helper::makeQObject<QSlider>( Qt::Orientation::Horizontal, this );
		_sliderBackgroundOpacity->setMinimum( 0 );
		_sliderBackgroundOpacity->setMaximum( 100 );
		_sliderBackgroundOpacity->setValue( 100 );

		connect( _sliderBackgroundOpacity, &QSlider::valueChanged, this, &ExportImage::_onBackgroundOpacity );

		_labelBackgroundOpacityValue = Helper::makeQObject<QLabel>( this );

		layoutBackgroundOpacity->addWidget( _labelBackgroundOpacity );
		layoutBackgroundOpacity->addWidget( _sliderBackgroundOpacity );
		layoutBackgroundOpacity->addWidget( _labelBackgroundOpacityValue );

		// Preview.
		auto * groupPreview	 = new QGroupBox( "Preview" );
		auto * layoutPreview = Helper::makeQObject<QHBoxLayout>( groupPreview );
		layoutPreview->setAlignment( Qt::AlignmentFlag::AlignCenter );
		groupPreview->setLayout( layoutPreview );
		groupPreview->setSizePolicy( QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding );
		_previewContainer = groupPreview;

		_preview = Helper::makeQObject<QLabel>( this );
		_preview->setAlignment( Qt::AlignmentFlag::AlignCenter );
		_preview->setSizePolicy( QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed );
		layoutPreview->addWidget( _preview );

		_previewTimer = Helper::makeQObject<QTimer>( this );
		_previewTimer->setSingleShot( true );
		_previewTimer->setInterval( PREVIEW_DEBOUNCE_MS );
		connect( _previewTimer, &QTimer::timeout, this, &ExportImage::_updatePreview );

		// Buttons.
		QDialogButtonBox * buttonBox = Helper::makeQObject<QDialogButtonBox>(
			QDialogButtonBox::StandardButton::Cancel | QDialogButtonBox::StandardButton::Save, this
		);

		// Layout.
		layout->addWidget( groupResolution );
		layout->addWidget( groupFormat );
		layout->addWidget( groupPreview, 1 );
		layout->addWidget( buttonBox );

		_onResolution( 1 );

		// Callbacks.
		connect(
			buttonBox,
			&QDialogButtonBox::accepted,
			[ this ]()
			{
				_previewTimer->stop();

				// Pop file dialog.
				const QString filters = _comboBoxFormat->currentIndex() == 0 ? "PNG (*.png)" : "JPEG (*.jpg *.jpeg)";
				const QString path	  = QFileDialog::getSaveFileName( this, "Save image", _lastExportFolder, filters );
				if ( path.isEmpty() )
				{
					return;
				}

				App::ACTION().execute<App::Action::IO::Snapshot>(
					path.toStdString(),
					_comboBoxFormat->currentIndex() == 0 ? Util::Image::E_FORMAT::PNG : Util::Image::E_FORMAT::JPEG,
					_spinBoxWidth->value(),
					_spinBoxHeight->value(),
					_comboBoxFormat->currentIndex() == 0 ? _sliderBackgroundOpacity->value() / 100.f : 1.f
				);

				const FilePath lastExportFolder = FilePath( path.toStdString() ).parent_path();
				_lastExportFolder				= QString::fromStdString( lastExportFolder.string() );

				SETTINGS().setValue( SETTING_KEY_WIDTH, _spinBoxWidth->value() );
				SETTINGS().setValue( SETTING_KEY_HEIGHT, _spinBoxHeight->value() );
				SETTINGS().setValue( SETTING_KEY_FORMAT, _comboBoxFormat->currentIndex() );
				SETTINGS().setValue( SETTING_KEY_OPACITY, _sliderBackgroundOpacity->value() );
				SETTINGS().setValue( SETTING_KEY_EXPORT_FOLDER, _lastExportFolder );

				accept();
			}
		);

		connect(
			buttonBox,
			&QDialogButtonBox::rejected,
			this,
			[ this ]()
			{
				_previewTimer->stop();
				reject();
			}
		);

		{
			const QSignalBlocker b( _spinBoxWidth );
			const QSignalBlocker b2( _spinBoxHeight );
			const QSignalBlocker b3( _comboBoxFormat );
			const QSignalBlocker b4( _sliderBackgroundOpacity );
			const QSignalBlocker b5( _comboBoxResolution );

			if ( SETTINGS().contains( SETTING_KEY_WIDTH ) )
			{
				_spinBoxWidth->setValue( SETTINGS().value( SETTING_KEY_WIDTH ).toInt() );
			}
			if ( SETTINGS().contains( SETTING_KEY_HEIGHT ) )
			{
				_spinBoxHeight->setValue( SETTINGS().value( SETTING_KEY_HEIGHT ).toInt() );
			}
			if ( SETTINGS().contains( SETTING_KEY_FORMAT ) )
			{
				_comboBoxFormat->setCurrentIndex( SETTINGS().value( SETTING_KEY_FORMAT ).toInt() );
			}
			if ( SETTINGS().contains( SETTING_KEY_OPACITY ) )
			{
				_sliderBackgroundOpacity->setValue( SETTINGS().value( SETTING_KEY_OPACITY ).toInt() );
			}
			if ( SETTINGS().contains( SETTING_KEY_EXPORT_FOLDER ) )
			{
				_lastExportFolder = SETTINGS().value( SETTING_KEY_EXPORT_FOLDER ).toString();
			}
		}

		_onFormat( _comboBoxFormat->currentIndex() );
		_onBackgroundOpacity();
		_onSize();
		QTimer::singleShot( 0, this, &ExportImage::_schedulePreviewUpdate );
	}

	void ExportImage::_onResolution( const int p_resolutionIndex )
	{
		if ( p_resolutionIndex == 0 )
		{
			return;
		}

		const int width	 = _comboBoxResolution->itemData( p_resolutionIndex, Qt::UserRole ).toInt();
		const int height = _comboBoxResolution->itemData( p_resolutionIndex, Qt::UserRole + 1 ).toInt();

		// Update size.
		const QSignalBlocker blockerWidth( _spinBoxWidth );
		const QSignalBlocker blockerHeight( _spinBoxHeight );
		_spinBoxWidth->setValue( width );
		_spinBoxHeight->setValue( height );

		const QSignalBlocker blockerResolution( _comboBoxResolution );
		_comboBoxResolution->setCurrentIndex( 0 );
		_onSize();
	}

	void ExportImage::_onSize()
	{
		const int width	 = _spinBoxWidth->value();
		const int height = _spinBoxHeight->value();

		// Update ratio.
		const double ratio = static_cast<double>( width ) / height;

		_labelRatioValue->setText( QString::number( ratio, 'f', 2 ) );
		const QSignalBlocker blocker( _sliderRatio );
		_sliderRatio->setValue( static_cast<int>( ratio * 10000 ) );
		_schedulePreviewUpdate();
	}

	void ExportImage::_onRatio()
	{
		const double ratio = _sliderRatio->value() / 10000.0;

		// Update Height.
		const int width	 = _spinBoxWidth->value();
		const int height = static_cast<int>( static_cast<double>( width ) / ratio );

		const QSignalBlocker blockerHeight( _spinBoxHeight );
		_spinBoxHeight->setValue( height );
		_labelRatioValue->setText( QString::number( ratio, 'f', 2 ) );
		_schedulePreviewUpdate();
	}

	void ExportImage::_onFormat( const int p_formatIndex )
	{
		const bool hasAlpha = p_formatIndex == static_cast<int>( Util::Image::E_FORMAT::PNG );
		_labelBackgroundOpacity->setVisible( hasAlpha );
		_sliderBackgroundOpacity->setVisible( hasAlpha );
		_labelBackgroundOpacityValue->setVisible( hasAlpha );
		_schedulePreviewUpdate();
	}

	void ExportImage::_onBackgroundOpacity()
	{
		_labelBackgroundOpacityValue->setText( QString::number( _sliderBackgroundOpacity->value() ) );
		_schedulePreviewUpdate();
	}

	void ExportImage::_schedulePreviewUpdate()
	{
		_updatePreviewSize();
		_previewTimer->start();
	}

	void ExportImage::_updatePreviewSize()
	{
		const int width	 = _spinBoxWidth->value();
		const int height = _spinBoxHeight->value();

		QSize available = _previewContainer->contentsRect().size();
		if ( _previewContainer->layout() != nullptr )
		{
			const QMargins margins = _previewContainer->layout()->contentsMargins();
			available.rwidth() -= margins.left() + margins.right();
			available.rheight() -= margins.top() + margins.bottom();
		}

		if ( available.width() <= 0 || available.height() <= 0 )
		{
			available = QSize( PREVIEW_FALLBACK_WIDTH, PREVIEW_FALLBACK_HEIGHT );
		}

		QSize previewSize( width, height );
		if ( previewSize.width() > available.width() || previewSize.height() > available.height() )
		{
			previewSize.scale( available, Qt::AspectRatioMode::KeepAspectRatio );
		}

		const int previewWidth	= previewSize.width();
		const int previewHeight = previewSize.height();

		_preview->setFixedSize( previewWidth, previewHeight );
	}

	void ExportImage::_updatePreview()
	{
		if ( _preview->width() <= 0 || _preview->height() <= 0 )
		{
			return;
		}

		const QSize			   previewSize = _preview->size();
		const size_t		   width	   = static_cast<size_t>( previewSize.width() );
		const size_t		   height	   = static_cast<size_t>( previewSize.height() );
		const Util::Resolution resolution  = { "Preview", width, height };
		const float			   backgroundOpacity
			= _comboBoxFormat->currentIndex() == 0 ? _sliderBackgroundOpacity->value() / 100.f : 1.f;
		std::vector<std::byte> image = App::RENDERER().snapshot( resolution, backgroundOpacity );

		if ( width == 0 || height == 0 || image.empty() )
		{
			return;
		}

		const QImage previewImage(
			reinterpret_cast<const uchar *>( image.data() ),
			static_cast<int>( width ),
			static_cast<int>( height ),
			QImage::Format::Format_RGBA8888
		);

		_preview->setPixmap( QPixmap::fromImage( previewImage.copy().flipped( Qt::Orientation::Vertical ) ) );
	}

} // namespace VTX::UI::QT::Dialog
