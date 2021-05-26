#include "information_widget.hpp"
#include "define.hpp"
#include "util/filesystem.hpp"
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <string>

namespace VTX::UI::Widget::Information
{
	InformationWidget::InformationWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {}

	void InformationWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		_mainWidget = new CustomWidget::DockWindowMainWidget<QWidget>( this );
		_mainWidget->setSizeHint( QSize( 500, 300 ) );
		_mainWidget->setMinimumSizeHint( QSize( 500, 300 ) );

		QLabel * const vtxLogo = new QLabel( _mainWidget );
		QPixmap		   logo	   = QPixmap( ":/sprite/logo.png", nullptr, Qt::ImageConversionFlag::ColorOnly );
		vtxLogo->setPixmap( logo );
		vtxLogo->setScaledContents( true );
		vtxLogo->setFixedSize( QSize( 64, 64 ) );

		QLabel * versionLabel = new QLabel( _mainWidget );
		versionLabel->setText( "Version: " + _getVersionText() );

		QLabel * licenseTitle = new QLabel( _mainWidget );
		licenseTitle->setText( "License:" );
		QScrollArea * const licenseScrollArea = new QScrollArea( _mainWidget );
		QLabel * const		licenseTxt		  = new QLabel( _mainWidget );
		licenseTxt->setText( _getLicenseText() );
		licenseTxt->setWordWrap( true );
		licenseScrollArea->setWidget( licenseTxt );

		QPushButton * const websiteButton = new QPushButton( _mainWidget );
		websiteButton->setText( "Website" );
		QPushButton * const gitButton = new QPushButton( _mainWidget );
		gitButton->setText( "Git" );
		QPushButton * const bugReportButton = new QPushButton( _mainWidget );
		bugReportButton->setText( "Report bug" );

		QLabel * libraryTitle = new QLabel( _mainWidget );
		libraryTitle->setText( "External libraries:" );
		QLabel * libraryDesc = new QLabel( _mainWidget );
		libraryDesc->setTextFormat( Qt::TextFormat::RichText );
		libraryDesc->setTextInteractionFlags( libraryDesc->textInteractionFlags()
											  | Qt::TextInteractionFlag::LinksAccessibleByMouse );
		libraryDesc->setOpenExternalLinks( true );
		libraryDesc->setText(
			"<a href = \"https://www.qt.io\">Qt</a> - <a href \"https://chemfiles.org\">Chemfiles</a>" );

		QVBoxLayout * const mainVerticalLayout = new QVBoxLayout( _mainWidget );
		QHBoxLayout * const headerLayout	   = new QHBoxLayout();
		QVBoxLayout * const vtxHeaderLayout	   = new QVBoxLayout();
		QHBoxLayout * const buttonsLayout	   = new QHBoxLayout();

		QLabel * vtxTitle = new QLabel( _mainWidget );
		vtxTitle->setText( "VTX" );

		buttonsLayout->addWidget( websiteButton );
		buttonsLayout->addWidget( gitButton );
		buttonsLayout->addWidget( bugReportButton );
		buttonsLayout->addStretch( 1000 );

		vtxHeaderLayout->addWidget( vtxTitle );
		vtxHeaderLayout->addWidget( versionLabel );
		vtxHeaderLayout->addSpacing( 10 );
		vtxHeaderLayout->addLayout( buttonsLayout );

		headerLayout->addWidget( vtxLogo );
		headerLayout->addSpacing( 10 );
		headerLayout->addLayout( vtxHeaderLayout );
		headerLayout->addStretch( 1000 );

		mainVerticalLayout->addItem( headerLayout );
		mainVerticalLayout->addWidget( licenseTitle );
		mainVerticalLayout->addWidget( licenseScrollArea );
		mainVerticalLayout->addWidget( libraryTitle );
		mainVerticalLayout->addWidget( libraryDesc );

		setWidget( _mainWidget );
	}

	void InformationWidget::_setupSlots() {};
	void InformationWidget::localize()
	{
		// Qt translate (not use currently)
		// setWindowTitle( QCoreApplication::translate( "ConsoleWidget", "Console", nullptr ) );
		setWindowTitle( "Information" );
	};

	QString InformationWidget::_getVersionText() const
	{
		const QString majorVersion = QString::fromStdString( std::to_string( VTX_VERSION_MAJOR ) );
		const QString minorVersion = QString::fromStdString( std::to_string( VTX_VERSION_MINOR ) );
		const QString revision	   = QString::fromStdString( std::to_string( VTX_VERSION_REVISION ) );

		return majorVersion + '.' + minorVersion + '.' + revision;
	}

	QString InformationWidget::_getLicenseText() const
	{
		return QString::fromStdString( Util::Filesystem::readPath( Util::Filesystem::LICENSE_PATH ) );
	}

} // namespace VTX::UI::Widget::Information
