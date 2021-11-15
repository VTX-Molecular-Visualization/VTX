#include "image_exporter.hpp"
#include "action/main.hpp"
#include "io/struct/image_export.hpp"
#include "setting.hpp"
#include "ui/main_window.hpp"
#include "ui/widget_factory.hpp"
#include "vtx_app.hpp"
#include <QFileDialog>
#include <QPushButton>

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

		_backgroundOpacitySlider = WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>(
			parameters, "BackgroundOpacitySlider" );
		_backgroundOpacitySlider->setMinMax( 0.f, 1.f );
		_addWidget( "Background opacity", _backgroundOpacitySlider, parametersLayout );

		_qualitySlider = WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>(
			parameters, "QualitySlider" );
		_qualitySlider->setMinMax( 0.f, 1.f );
		_addWidget( "Quality", _qualitySlider, parametersLayout );

		_dialogButtons
			= new QDialogButtonBox( QDialogButtonBox::StandardButton::Cancel | QDialogButtonBox::StandardButton::Save,
									Qt::Orientation::Horizontal,
									this );

		verticalLayout->addWidget( parameters );
		verticalLayout->addStretch();
		verticalLayout->addWidget( _dialogButtons );

		_dialogButtons->setFocus();
	}

	void ImageExporter::_setupSlots()
	{
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
	}

	void ImageExporter::_addWidget( const QString & p_label, QWidget * const p_setting, QGridLayout * const p_layout )
	{
		QLabel * const labelWidget = new QLabel( p_setting );
		labelWidget->setText( p_label );

		const int row = p_layout->rowCount();

		p_layout->addWidget( labelWidget, row, 0 );
		p_layout->addWidget( p_setting, row, 1 );
	}
	void ImageExporter::_addWidget( QWidget * const p_setting, QGridLayout * const p_layout )
	{
		const int row = p_layout->rowCount();
		p_layout->addWidget( p_setting, row, 0, 1, 2 );
	}

	void ImageExporter::cancelAction() { close(); }
	void ImageExporter::saveAction()
	{
		const IO::Struct::ImageExport exportData
			= IO::Struct::ImageExport( IO::Struct::ImageExport::RESOLUTION( _resolutionWidget->currentIndex() ),
									   _backgroundOpacitySlider->getValue(),
									   _qualitySlider->getValue() );

		setEnabled( false );

		const bool imageExported = UI::Dialog::openExportImageDialog( exportData );

		if ( imageExported )
			close();

		setEnabled( true );
	}

} // namespace VTX::UI::Widget::Dialog
