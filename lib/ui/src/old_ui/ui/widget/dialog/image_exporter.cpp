#include "ui/old_ui/ui/widget/dialog/image_exporter.hpp"
#include "ui/old_ui/ui/dialog.hpp"
#include "ui/old_ui/ui/main_window.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/old_ui/vtx_app.hpp"
#include <QFileDialog>
#include <QPushButton>
#include <app/action/main.hpp>
#include <app/old_app/io/struct/image_export.hpp>
#include <app/old_app/setting.hpp>
#include <app/old_app/spec.hpp>

namespace VTX::UI::Widget::Dialog
{
	ImageExporter & ImageExporter::_getInstance()
	{
		if ( _instance == nullptr )
		{
			_instance = WidgetFactory::get().instantiateWidget<Dialog::ImageExporter>( &VTXApp::get().getMainWindow(),
																					   "imageExporterDialog" );
		}

		return *_instance;
	}
	ImageExporter::ImageExporter( QWidget * p_parent ) : BaseManualWidget( p_parent ) {}

	void ImageExporter::openDialog() { _getInstance().show(); }

	void ImageExporter::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );
		setWindowModality( Qt::WindowModality::ApplicationModal );
		setWindowFlags( windowFlags() & ~Qt::WindowFlags::enum_type::WindowContextHelpButtonHint );

		QVBoxLayout * const verticalLayout	 = new QVBoxLayout( this );
		QWidget *			parameters		 = new QWidget( this );
		QGridLayout * const parametersLayout = new QGridLayout( parameters );

		_resolutionWidget = new QComboBox( parameters );
		for ( const std::string & resolutionStr : IO::Struct::ImageExport::RESOLUTION_STR )
			_resolutionWidget->addItem( QString::fromStdString( resolutionStr ) );
		_addWidget( "Resolution", _resolutionWidget, parametersLayout );

		_customResolutionWidget = new QWidget( parameters );

		_customResolutionLabel = new QLabel( parameters );
		_customResolutionLabel->setText( "Custom Resolution" );
		_resolutionWidthWidget = WidgetFactory::get().instantiateWidget<CustomWidget::IntegerFieldDraggableWidget>(
			_customResolutionWidget, "ResolutionWidth" );
		const std::pair<int, int> widthRange = IO::Struct::ImageExport::getSnapshotWidthRange();
		_resolutionWidthWidget->setMinMax( widthRange.first, widthRange.second );
		_resolutionHeightWidget = WidgetFactory::get().instantiateWidget<CustomWidget::IntegerFieldDraggableWidget>(
			_customResolutionWidget, "ResolutionHeight" );
		const std::pair<int, int> heightRange = IO::Struct::ImageExport::getSnapshotHeightRange();
		_resolutionHeightWidget->setMinMax( heightRange.first, heightRange.second );

		QLabel * widthLabel = new QLabel( _customResolutionWidget );
		widthLabel->setText( "Width" );
		QLabel * heightLabel = new QLabel( _customResolutionWidget );
		heightLabel->setText( "Height" );

		QHBoxLayout * resolutionLayout = new QHBoxLayout( _customResolutionWidget );
		resolutionLayout->addWidget( widthLabel, 1 );
		resolutionLayout->addWidget( _resolutionWidthWidget, 100 );
		resolutionLayout->addSpacing( 10 );
		resolutionLayout->addWidget( heightLabel, 1 );
		resolutionLayout->addWidget( _resolutionHeightWidget, 100 );
		_addWidget( _customResolutionLabel, _customResolutionWidget, parametersLayout );

		_backgroundOpacitySlider = WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>(
			parameters, "BackgroundOpacitySlider" );
		_backgroundOpacitySlider->setMinMax( 0.f, 1.f );
		_addWidget( "Background opacity", _backgroundOpacitySlider, parametersLayout );

		_qualitySlider = WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>(
			parameters, "QualitySlider" );
		_qualitySlider->setMinMax( 0.f, 1.f );
		_addWidget( "Quality", _qualitySlider, parametersLayout );

		_previewWidget
			= WidgetFactory::get().instantiateWidget<CustomWidget::RenderPreviewWidget>( parameters, "PreviewWidget" );
		_previewWidget->setSizePolicy( QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred );

		_dialogButtons
			= new QDialogButtonBox( QDialogButtonBox::StandardButton::Cancel | QDialogButtonBox::StandardButton::Save,
									Qt::Orientation::Horizontal,
									this );

		verticalLayout->addWidget( parameters );
		verticalLayout->addStretch();
		verticalLayout->addWidget( _previewWidget, 0, Qt::AlignCenter );
		verticalLayout->addStretch();
		verticalLayout->addWidget( _dialogButtons );

		_dialogButtons->setFocus();
	}

	void ImageExporter::_setupSlots()
	{
		connect( _resolutionWidget,
				 QOverload<int>::of( &QComboBox::currentIndexChanged ),
				 this,
				 &ImageExporter::_resolutionChange );

		connect( _resolutionWidthWidget,
				 &CustomWidget::IntegerFieldDraggableWidget::onValueChange,
				 this,
				 &ImageExporter::_resolutionWidthChange );
		connect( _resolutionHeightWidget,
				 &CustomWidget::IntegerFieldDraggableWidget::onValueChange,
				 this,
				 &ImageExporter::_resolutionHeightChange );

		connect( _backgroundOpacitySlider,
				 &CustomWidget::FloatFieldSliderWidget::onValueChange,
				 this,
				 &ImageExporter::_backgroundOpacityChange );
		connect( _qualitySlider,
				 &CustomWidget::FloatFieldSliderWidget::onValueChange,
				 this,
				 &ImageExporter::_qualityChange );

		QPushButton * cancelButton = _dialogButtons->button( QDialogButtonBox::StandardButton::Cancel );
		connect( cancelButton, &QPushButton::clicked, this, &ImageExporter::cancelAction );

		QPushButton * openButton = _dialogButtons->button( QDialogButtonBox::StandardButton::Save );
		connect( openButton, &QPushButton::clicked, this, &ImageExporter::saveAction );
	}
	void ImageExporter::localize() { this->setWindowTitle( "Image Exporter" ); }

	void ImageExporter::showEvent( QShowEvent * p_event )
	{
		BaseManualWidget::showEvent( p_event );

		_refresh();
		_dialogButtons->setFocus();
	}

	void ImageExporter::_refresh()
	{
		_backgroundOpacitySlider->setValue( VTX_SETTING().getSnapshotBackgroundOpacity() );
		_resolutionWidget->setCurrentIndex( int( VTX_SETTING().getSnapshotResolution() ) );
		_qualitySlider->setValue( VTX_SETTING().getSnapshotQuality() );

		const bool displayCustomResolution
			= VTX_SETTING().getSnapshotResolution() == IO::Struct::ImageExport::RESOLUTION::Free;
		_customResolutionLabel->setVisible( displayCustomResolution );
		_customResolutionWidget->setVisible( displayCustomResolution );

		std::pair<int, int> freeResolution
			= IO::Struct::ImageExport::getSize( IO::Struct::ImageExport::RESOLUTION::Free );
		_resolutionWidthWidget->setValue( freeResolution.first );
		_resolutionHeightWidget->setValue( freeResolution.second );

		_refreshPreview();
	}

	void ImageExporter::_refreshPreview()
	{
		const IO::Struct::ImageExport::RESOLUTION resolution
			= IO::Struct::ImageExport::RESOLUTION( _resolutionWidget->currentIndex() );

		std::pair<int, int> resolutionSize;

		if ( resolution == IO::Struct::ImageExport::RESOLUTION::Free )
		{
			resolutionSize.first  = _resolutionWidthWidget->getValue();
			resolutionSize.second = _resolutionHeightWidget->getValue();
		}
		else
		{
			resolutionSize = IO::Struct::ImageExport::getSize( resolution );
		}

		const IO::Struct::ImageExport previewExportData = IO::Struct::ImageExport(
			resolutionSize, _backgroundOpacitySlider->getValue(), _qualitySlider->getValue() );

		_previewWidget->takeSnapshot( previewExportData );
	}

	void ImageExporter::_addWidget( const QString & p_label, QWidget * const p_setting, QGridLayout * const p_layout )
	{
		QLabel * const labelWidget = new QLabel( p_setting );
		labelWidget->setText( p_label );

		_addWidget( labelWidget, p_setting, p_layout );
	}
	void ImageExporter::_addWidget( QLabel * const p_label, QWidget * const p_setting, QGridLayout * const p_layout )
	{
		const int row = p_layout->rowCount();

		p_layout->addWidget( p_label, row, 0 );
		p_layout->addWidget( p_setting, row, 1 );
	}
	void ImageExporter::_addWidget( QWidget * const p_setting, QGridLayout * const p_layout )
	{
		const int row = p_layout->rowCount();
		p_layout->addWidget( p_setting, row, 0, 1, 2 );
	}

	void ImageExporter::_resolutionChange( const int p_resolutionIndex )
	{
		const IO::Struct::ImageExport::RESOLUTION resolution
			= IO::Struct::ImageExport::RESOLUTION( _resolutionWidget->currentIndex() );

		const bool displayCustomResolution = resolution == IO::Struct::ImageExport::RESOLUTION::Free;
		_customResolutionLabel->setVisible( displayCustomResolution );
		_customResolutionWidget->setVisible( displayCustomResolution );

		_refreshPreview();
	}
	void ImageExporter::_resolutionWidthChange( const int p_value ) { _refreshPreview(); }
	void ImageExporter::_resolutionHeightChange( const int p_value ) { _refreshPreview(); }
	void ImageExporter::_backgroundOpacityChange( const float p_opacity ) { _refreshPreview(); }
	void ImageExporter::_qualityChange( const float p_quality ) { _refreshPreview(); }

	void ImageExporter::cancelAction() { close(); }
	void ImageExporter::saveAction()
	{
		IO::Struct::ImageExport exportData
			= IO::Struct::ImageExport( IO::Struct::ImageExport::RESOLUTION( _resolutionWidget->currentIndex() ),
									   _backgroundOpacitySlider->getValue(),
									   _qualitySlider->getValue() );

		const IO::Struct::ImageExport::RESOLUTION resolution
			= IO::Struct::ImageExport::RESOLUTION( _resolutionWidget->currentIndex() );

		if ( resolution == IO::Struct::ImageExport::RESOLUTION::Free )
		{
			std::pair<int, int> resolutionSize;

			resolutionSize.first  = _resolutionWidthWidget->getValue();
			resolutionSize.second = _resolutionHeightWidget->getValue();
			exportData.setSize( resolutionSize );
		}

		setEnabled( false );

		const bool imageExported = UI::Dialog::openExportImageDialog( exportData );

		if ( imageExported )
			close();

		setEnabled( true );
	}

} // namespace VTX::UI::Widget::Dialog
