#include "ui/qt/dialog/export_image.hpp"
#include "app/application/scene.hpp"
#include "ui/qt/application.hpp"
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <app/application/scene.hpp>
#include <app/component/render/camera.hpp>
#include <app/core/renderer/renderer_system.hpp>

namespace VTX::UI::QT::Dialog
{

	ExportImage::ExportImage() : BaseWidget<ExportImage, QDialog>( APP_QT::getMainWindow() )
	{
		setWindowTitle( "Export image" );
		setFixedSize( 500, 600 );

		auto * layout = new QVBoxLayout( this );

		// Resolution.
		auto * layoutResolution = new QHBoxLayout( this );
		layoutResolution->setAlignment( Qt::AlignmentFlag::AlignCenter );

		auto * labelResolution = new QLabel( "Apply resolution preset", this );
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

		layoutResolution->addWidget( labelResolution );
		layoutResolution->addWidget( _comboBoxResolution );

		// Size widgets.
		auto * layoutSize = new QHBoxLayout( this );
		layoutSize->setAlignment( Qt::AlignmentFlag::AlignCenter );

		auto * labelWidth = new QLabel( "Width", this );
		_spinBoxWidth	  = new QSpinBox( this );
		_spinBoxWidth->setMinimum( 100 );
		_spinBoxWidth->setMaximum( _MAX_TEXTURE_SIZE );
		_spinBoxWidth->setValue( int( camera.getScreenWidth() ) );

		auto * labelHeight = new QLabel( "Height", this );
		_spinBoxHeight	   = new QSpinBox( this );
		_spinBoxHeight->setMinimum( 100 );
		_spinBoxHeight->setMaximum( _MAX_TEXTURE_SIZE );
		_spinBoxHeight->setValue( int( camera.getScreenHeight() ) );

		connect( _spinBoxWidth, QOverload<int>::of( &QSpinBox::valueChanged ), this, &ExportImage::_onSize );
		connect( _spinBoxHeight, QOverload<int>::of( &QSpinBox::valueChanged ), this, &ExportImage::_onSize );

		layoutSize->addWidget( labelWidth );
		layoutSize->addWidget( _spinBoxWidth );
		layoutSize->addWidget( labelHeight );
		layoutSize->addWidget( _spinBoxHeight );

		// Ratio widgets.
		auto * layoutRatio = new QHBoxLayout( this );
		layoutRatio->setAlignment( Qt::AlignmentFlag::AlignCenter );

		auto * labelRatio = new QLabel( "Ratio adjustement", this );
		_labelRatio		  = new QLabel( this );
		_labelRatio->setText(
			QString::number( float( camera.getScreenWidth() ) / float( camera.getScreenHeight() ), 'f', 2 )
		);

		/*
		_spinBoxRatio->setMinimum( _RATIO_MIN );
		_spinBoxRatio->setMaximum( _RATIO_MAX );
		_spinBoxRatio->setValue( double( camera.getScreenWidth() ) / double( camera.getScreenHeight() ) );

		connect( _spinBoxRatio, QOverload<double>::of( &QDoubleSpinBox::valueChanged ), this, &ExportImage::_onRatio );
		*/

		_sliderRatio = new QSlider( Qt::Orientation::Horizontal, this );
		_sliderRatio->setMinimum( int( _RATIO_MIN * 100 ) );
		_sliderRatio->setMaximum( int( _RATIO_MAX * 100 ) );
		_sliderRatio->setValue( int( camera.getScreenWidth() / float( camera.getScreenHeight() ) * 100 ) );

		connect( _sliderRatio, &QSlider::valueChanged, this, &ExportImage::_onRatio );

		layoutRatio->addWidget( labelRatio );
		layoutRatio->addWidget( _labelRatio );
		layoutRatio->addWidget( _sliderRatio );

		// Preview.
		_preview = new QLabel( this );

		// Set size policy.
		_preview->setSizePolicy( QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding );
		_preview->setAlignment( Qt::AlignmentFlag::AlignCenter );

		// Buttons.
		QDialogButtonBox * buttonBox = new QDialogButtonBox(
			QDialogButtonBox::StandardButton::Cancel | QDialogButtonBox::StandardButton::Save, this
		);

		// Layout.
		layout->addLayout( layoutResolution );
		layout->addLayout( layoutSize );
		layout->addLayout( layoutRatio );
		layout->addWidget( _preview );
		layout->addWidget( buttonBox );

		setLayout( layout );

		// Updae preview.
		_updatePreview();

		// Callbacks.
		connect(
			buttonBox->button( QDialogButtonBox::StandardButton::Open ),
			&QPushButton::clicked,
			[ this ]() {

			}
		);

		connect(
			buttonBox->button( QDialogButtonBox::StandardButton::Cancel ),
			&QPushButton::clicked,
			[ this ]() { close(); }
		);
	}

	void ExportImage::_onResolution()
	{
		const int resolutionIndex = _comboBoxResolution->currentIndex();

		if ( resolutionIndex == 0 )
		{
			return;
		}

		const auto & resolution = _RESOLUTIONS[ resolutionIndex - 1 ];

		// Update size.
		_spinBoxWidth->setValue( int( resolution.width ) );
		_spinBoxHeight->setValue( int( resolution.height ) );

		// Update ratio.
		const float ratio = float( resolution.width ) / float( resolution.height );
		_labelRatio->setText( QString::number( ratio, 'f', 2 ) );
		_sliderRatio->setValue( int( ratio * 100 ) );

	} // namespace VTX::UI::QT::Dialog

	void ExportImage::_onSize()
	{
		const int width	 = _spinBoxWidth->value();
		const int height = _spinBoxHeight->value();

		// Reset resolution.
		_comboBoxResolution->setCurrentIndex( 0 );

		// Update ratio.
		const float ratio = float( width ) / height;
		//_labelRatio->setValue( double( ratio ) );

		_labelRatio->setText( QString::number( ratio, 'f', 2 ) );
		_sliderRatio->setValue( int( ratio * 100 ) );

		_updatePreview();
	}

	void ExportImage::_onRatio()
	{
		// const float ratio = float( _labelRatio->value() );
		const float ratio = _sliderRatio->value() / 100.0f;

		// Reset resolution.
		_comboBoxResolution->setCurrentIndex( 0 );

		// Update Height.
		const int width	 = _spinBoxWidth->value();
		const int height = int( float( width ) / ratio );
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
		auto &			   renderer = App::RENDERER_SYSTEM().facade();
		const auto &	   camera	= App::SCENE().getCamera();
		std::vector<uchar> image;
		renderer.snapshot( image, width, height, camera.getFov(), camera.getNear(), camera.getFar() );

		QImage qImage( image.data(), width, height, QImage::Format::Format_RGBA8888 );
		qImage = qImage.mirrored( false, true );
		_preview->setPixmap( QPixmap::fromImage( qImage ) );
	}

} // namespace VTX::UI::QT::Dialog
