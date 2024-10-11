#include "ui/qt/dialog/export_image.hpp"
#include "app/application/scene.hpp"
#include "ui/qt/application.hpp"
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QPushButton>
#include <QToolTip>
#include <QVBoxLayout>
#include <app/action/export.hpp>
#include <app/application/scene.hpp>
#include <app/component/render/camera.hpp>
#include <app/core/renderer/renderer_system.hpp>

namespace VTX::UI::QT::Dialog
{

	ExportImage::ExportImage()
	{
		setWindowTitle( "Export image" );
		setFixedSize( 500, 600 );

		auto * layout = new QVBoxLayout( this );

		// Resolution.
		auto * groupResolution		 = new QGroupBox( "Resolution" );
		auto * layoutGroupResolution = new QVBoxLayout( this );
		groupResolution->setLayout( layoutGroupResolution );
		auto * layoutPresetSize = new QHBoxLayout( this );

		layoutPresetSize->setAlignment( Qt::AlignmentFlag::AlignLeft );

		auto * labelResolution = new QLabel( "Preset", this );
		_comboBoxResolution	   = new QComboBox( this );
		_comboBoxResolution->addItem( "-select-" );
		_comboBoxResolution->setInsertPolicy( QComboBox::InsertPolicy::NoInsert );

		auto & camera			 = App::SCENE().getCamera();
		_RESOLUTIONS[ 0 ].width	 = camera.getScreenWidth();
		_RESOLUTIONS[ 0 ].height = camera.getScreenHeight();

		for ( const auto & resolution : _RESOLUTIONS )
		{
			if ( resolution.width > _MAX_TEXTURE_SIZE || resolution.height > _MAX_TEXTURE_SIZE )
			{
				continue;
			}

			QString text = QString::fromStdString( resolution.name.data() );
			text += QString( " (%1x%2)" ).arg( resolution.width ).arg( resolution.height );
			_comboBoxResolution->addItem( text );
		}

		connect(
			_comboBoxResolution,
			QOverload<int>::of( &QComboBox::currentIndexChanged ),
			this,
			&ExportImage::_onResolution
		);

		layoutPresetSize->addWidget( labelResolution );
		layoutPresetSize->addWidget( _comboBoxResolution );

		// Size widgets.
		auto * labelWidth = new QLabel( "Width", this );
		_spinBoxWidth	  = new QSpinBox( this );
		_spinBoxWidth->setMinimum( 100 );
		_spinBoxWidth->setMaximum( _MAX_TEXTURE_SIZE );
		_spinBoxWidth->setSizePolicy( QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed );

		auto * labelHeight = new QLabel( "Height", this );
		_spinBoxHeight	   = new QSpinBox( this );
		_spinBoxHeight->setMinimum( 100 );
		_spinBoxHeight->setMaximum( _MAX_TEXTURE_SIZE );
		_spinBoxHeight->setSizePolicy( QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed );

		connect( _spinBoxWidth, QOverload<int>::of( &QSpinBox::valueChanged ), this, &ExportImage::_onSize );
		connect( _spinBoxHeight, QOverload<int>::of( &QSpinBox::valueChanged ), this, &ExportImage::_onSize );

		layoutPresetSize->addWidget( labelWidth );
		layoutPresetSize->addWidget( _spinBoxWidth );
		layoutPresetSize->addWidget( labelHeight );
		layoutPresetSize->addWidget( _spinBoxHeight );

		// Ratio widgets.
		auto * layoutRatio = new QHBoxLayout( this );
		layoutRatio->setAlignment( Qt::AlignmentFlag::AlignCenter );

		auto * labelRatio = new QLabel( "Ratio", this );
		labelRatio->setToolTip( "Adjust height from ratio (ratio = width / height)" );

		_labelRatioValue = new QLabel( this );

		_sliderRatio = new QSlider( Qt::Orientation::Horizontal, this );
		_sliderRatio->setMinimum( int( _RATIO_MIN * 10000 ) );
		_sliderRatio->setMaximum( int( _RATIO_MAX * 10000 ) );

		connect( _sliderRatio, &QSlider::valueChanged, this, &ExportImage::_onRatio );

		layoutRatio->addWidget( labelRatio );
		layoutRatio->addWidget( _sliderRatio );
		layoutRatio->addWidget( _labelRatioValue );

		layoutGroupResolution->addLayout( layoutPresetSize );
		layoutGroupResolution->addLayout( layoutRatio );

		// Format.
		auto * groupFormat	= new QGroupBox( "Format" );
		auto * layoutFormat = new QVBoxLayout( this );
		groupFormat->setLayout( layoutFormat );

		_comboBoxFormat = new QComboBox( this );
		_comboBoxFormat->addItem( _FORMATS[ 0 ].data() );
		_comboBoxFormat->addItem( _FORMATS[ 1 ].data() );

		connect(
			_comboBoxFormat, QOverload<int>::of( &QComboBox::currentIndexChanged ), this, &ExportImage::_onFormat
		);

		// Backgroud opacity.
		_layoutBackgroundOpacity = new QHBoxLayout( this );
		_labelBackgroundOpacity	 = new QLabel( "Background opacity", this );
		_sliderBackgroundOpacity = new QSlider( Qt::Orientation::Horizontal, this );
		_sliderBackgroundOpacity->setMinimum( 0 );
		_sliderBackgroundOpacity->setMaximum( 100 );
		_sliderBackgroundOpacity->setValue( 100 );

		connect( _sliderBackgroundOpacity, &QSlider::valueChanged, this, &ExportImage::_onBackgroundOpacity );

		_labelBackgroundOpacityValue = new QLabel( this );

		_layoutBackgroundOpacity->addWidget( _labelBackgroundOpacity );
		_layoutBackgroundOpacity->addWidget( _sliderBackgroundOpacity );
		_layoutBackgroundOpacity->addWidget( _labelBackgroundOpacityValue );

		layoutFormat->addWidget( _comboBoxFormat );
		layoutFormat->addLayout( _layoutBackgroundOpacity );

		// Preview.
		_preview = new QLabel( this );
		_preview->setSizePolicy( QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding );
		_preview->setAlignment( Qt::AlignmentFlag::AlignCenter );

		//_preview->setAttribute( Qt::WA_TranslucentBackground, true );
		//_preview->setWindowOpacity( 0.5 );

		// Buttons.
		QDialogButtonBox * buttonBox = new QDialogButtonBox(
			QDialogButtonBox::StandardButton::Cancel | QDialogButtonBox::StandardButton::Save, this
		);

		// Layout.
		layout->addWidget( groupResolution );
		layout->addWidget( groupFormat );
		layout->addWidget( _preview );
		layout->addWidget( buttonBox );

		// Trigger default values.
		_onResolution( 1 );
		_onFormat( 0 );
		_onBackgroundOpacity();

		// Restore settings.
		restore();

		// Callbacks.
		connect(
			buttonBox->button( QDialogButtonBox::StandardButton::Save ),
			&QPushButton::clicked,
			[ this ]()
			{
				// Pop file dialog.
				QString filters = _comboBoxFormat->currentIndex() == 0 ? "*.png" : "*.jpg, *.jpeg";
				QString path	= QFileDialog::getSaveFileName( this, "Save image", _lastExportFolder, filters );
				if ( path.isEmpty() )
				{
					return;
				}

				App::ACTION_SYSTEM().execute<App::Action::Export::Snapshot>(
					path.toStdString(),
					_comboBoxFormat->currentIndex() == 0 ? Util::Image::E_FORMAT::PNG : Util::Image::E_FORMAT::JPEG,
					_spinBoxWidth->value(),
					_spinBoxHeight->value()
				);

				FilePath lastExportFolder = FilePath( path.toStdString() );
				lastExportFolder		  = lastExportFolder.parent_path();
				_lastExportFolder		  = QString::fromStdString( lastExportFolder.string() );

				save();
				close();
			}
		);

		connect(
			buttonBox->button( QDialogButtonBox::StandardButton::Cancel ),
			&QPushButton::clicked,
			[ this ]()
			{
				save();
				close();
			}
		);
	}

	void ExportImage::_onResolution( const int p_resolutionIndex )
	{
		if ( p_resolutionIndex == 0 )
		{
			return;
		}

		const auto & resolution = _RESOLUTIONS[ p_resolutionIndex - 1 ];

		// Update size.
		_spinBoxWidth->setValue( int( resolution.width ) );
		_spinBoxHeight->setValue( int( resolution.height ) );

		_comboBoxResolution->setCurrentIndex( 0 );

	} // namespace VTX::UI::QT::Dialog

	void ExportImage::_onSize()
	{
		const int width	 = _spinBoxWidth->value();
		const int height = _spinBoxHeight->value();

		// Update ratio.
		const double ratio = double( width ) / height;

		_labelRatioValue->setText( QString::number( ratio, 'f', 2 ) );
		_sliderRatio->setValue( int( ratio * 10000 ) );

		// Delay because widget sizes are not updated yet.
		APP().onEndOfFrameOneShot += [ this ]() { _updatePreview(); };
	}

	void ExportImage::_onRatio()
	{
		const double ratio = _sliderRatio->value() / 10000.0;

		// Update Height.
		const int width	 = _spinBoxWidth->value();
		const int height = int( double( width ) / ratio );
		_spinBoxHeight->setValue( height );
	}

	void ExportImage::_updatePreview()
	{
		// Size.
		int width  = _spinBoxWidth->value();
		int height = _spinBoxHeight->value();

		// Preview size.
		const int widgetWidth = _preview->size().width() - _preview->parentWidget()->layout()->contentsMargins().left()
								- _preview->parentWidget()->layout()->contentsMargins().right();
		const int widgetHeight = _preview->size().height() - _preview->parentWidget()->layout()->contentsMargins().top()
								 - _preview->parentWidget()->layout()->contentsMargins().bottom();

		// Update renderer size from widget max size with same ratio.
		const float ratio = float( width ) / float( height );
		if ( width > widgetHeight )
		{
			width  = widgetWidth;
			height = int( float( width ) / ratio );
		}
		if ( height > widgetHeight )
		{
			height = widgetHeight;
			width  = int( float( height ) * ratio );
		}

		// Get preview image.
		const auto &	   camera = App::SCENE().getCamera();
		std::vector<uchar> image;
		App::RENDERER_SYSTEM().snapshot( image, width, height, camera.getFov(), camera.getNear(), camera.getFar() );

		QImage qImage( image.data(), width, height, QImage::Format::Format_RGBA8888 );
		qImage = qImage.mirrored( false, true );
		_preview->setPixmap( QPixmap::fromImage( qImage ) );
	}

	void ExportImage::_onFormat( const int p_formatIndex )
	{
		const bool hasAlpha = p_formatIndex == int( Util::Image::E_FORMAT::PNG );
		_labelBackgroundOpacity->setVisible( hasAlpha );
		_sliderBackgroundOpacity->setVisible( hasAlpha );
		_labelBackgroundOpacityValue->setVisible( hasAlpha );
	}

	void ExportImage::_onBackgroundOpacity()
	{
		_labelBackgroundOpacityValue->setText( QString::number( _sliderBackgroundOpacity->value() ) );
		//_updatePreview();
	}

	void ExportImage::save()
	{
		SETTINGS.setValue( _SETTING_KEY_WIDTH, _spinBoxWidth->value() );
		SETTINGS.setValue( _SETTING_KEY_HEIGHT, _spinBoxHeight->value() );
		SETTINGS.setValue( _SETTING_KEY_FORMAT, _comboBoxFormat->currentIndex() );
		SETTINGS.setValue( _SETTING_KEY_OPACITY, _sliderBackgroundOpacity->value() );
		SETTINGS.setValue( _SETTING_KEY_FOLDER, _lastExportFolder );
	}

	void ExportImage::restore()
	{
		if ( SETTINGS.contains( _SETTING_KEY_WIDTH ) )
		{
			_spinBoxWidth->setValue( SETTINGS.value( _SETTING_KEY_WIDTH ).toInt() );
		}
		if ( SETTINGS.contains( _SETTING_KEY_HEIGHT ) )
		{
			_spinBoxHeight->setValue( SETTINGS.value( _SETTING_KEY_HEIGHT ).toInt() );
		}
		if ( SETTINGS.contains( _SETTING_KEY_FORMAT ) )
		{
			_comboBoxFormat->setCurrentIndex( SETTINGS.value( _SETTING_KEY_FORMAT ).toInt() );
		}
		if ( SETTINGS.contains( _SETTING_KEY_OPACITY ) )
		{
			_sliderBackgroundOpacity->setValue( SETTINGS.value( _SETTING_KEY_OPACITY ).toInt() );
		}
		if ( SETTINGS.contains( _SETTING_KEY_FOLDER ) )
		{
			_lastExportFolder = SETTINGS.value( _SETTING_KEY_FOLDER ).toString();
		}
	}
} // namespace VTX::UI::QT::Dialog
