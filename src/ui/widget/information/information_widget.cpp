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

		setWindowFlag( Qt::WindowType::WindowContextHelpButtonHint, false );

		QLabel * const vtxLogo = new QLabel( this );
		QPixmap		   logo	   = QPixmap( ":/sprite/logo.png", nullptr, Qt::ImageConversionFlag::ColorOnly );
		vtxLogo->setPixmap( logo );
		vtxLogo->setScaledContents( true );
		vtxLogo->setFixedSize( QSize( 64, 64 ) );

		QLabel * versionLabel = new QLabel( this );
		versionLabel->setText( "Version: " + _getVersionText() );

		QLabel * licenseTitle = new QLabel( this );
		licenseTitle->setText( "License:" );
		QScrollArea * const licenseScrollArea = new QScrollArea( this );
		QLabel * const		licenseTxt		  = new QLabel( this );
		licenseTxt->setText( _getLicenseText() );
		licenseTxt->setWordWrap( true );
		licenseScrollArea->setWidget( licenseTxt );

		_websiteButton = new QPushButton( this );
		_websiteButton->setText( "Website" );
		_gitButton = new QPushButton( this );
		_gitButton->setText( "Git" );
		_docButton = new QPushButton( this );
		_docButton->setText( "Doc" );
		_bugReportButton = new QPushButton( this );
		_bugReportButton->setText( "Report bug" );

		QLabel * libraryTitle = new QLabel( this );
		libraryTitle->setText( "External libraries:" );
		QLabel * libraryDesc = new QLabel( this );
		libraryDesc->setTextInteractionFlags( libraryDesc->textInteractionFlags()
											  | Qt::TextInteractionFlag::LinksAccessibleByMouse );
		libraryDesc->setOpenExternalLinks( true );
		libraryDesc->setWordWrap( true );
		libraryDesc->setFrameShape( QFrame::Shape::Panel );
		libraryDesc->setFrameShadow( QFrame::Shadow::Sunken );
		libraryDesc->setText(
			"<a href = \"https://www.qt.io\">Qt</a> - <a href \"https://chemfiles.org\">Chemfiles</a>" );

		QLabel * iconAuthorsTitle = new QLabel( this );
		iconAuthorsTitle->setText( "Icon authors:" );
		QLabel * iconAuthors = new QLabel( this );
		iconAuthors->setTextInteractionFlags( libraryDesc->textInteractionFlags()
											  | Qt::TextInteractionFlag::LinksAccessibleByMouse );
		iconAuthors->setOpenExternalLinks( true );
		iconAuthors->setWordWrap( true );
		iconAuthors->setFrameShape( QFrame::Shape::Panel );
		iconAuthors->setFrameShadow( QFrame::Shadow::Sunken );
		iconAuthors->setText(
			"<a href = \"https://www.flaticon.com/authors/alfredo-hernandez\">alfredo-hernandez</a> - "
			"<a href = \"https://www.flaticon.com/authors/dinosoftlabs\">dinosoftlabs</a> - "
			"<a href = \"https://www.flaticon.com/authors/freepik\">Freepik</a> - "
			"<a href = \"https://www.flaticon.com/authors/good-ware\">good-ware</a> - "
			"<a href = \"https://www.flaticon.com/authors/google\">google</a> - "
			"<a href = \"https://www.flaticon.com/authors/itim2101\">itim2101</a> - "
			"<a href = \"https://www.flaticon.com/authors/kiranshastry\">kiranshastry</a> - "
			"<a href = \"https://www.flaticon.com/authors/mavadee\">mavadee</a> - "
			"<a href = \"https://www.flaticon.com/authors/pixel-perfect\">pixel-perfect</a> - "
			"<a href = \"https://www.flaticon.com/authors/roundicons\">roundicons</a> - "
			"<a href = \"https://www.flaticon.com/authors/vectors-market\">vectors-market</a>" );

		QVBoxLayout * const mainVerticalLayout = new QVBoxLayout( this );
		QHBoxLayout * const headerLayout	   = new QHBoxLayout();
		QVBoxLayout * const vtxHeaderLayout	   = new QVBoxLayout();
		QHBoxLayout * const buttonsLayout	   = new QHBoxLayout();

		QLabel * vtxTitle = new QLabel( this );
		vtxTitle->setText( "VTX" );

		buttonsLayout->addWidget( _websiteButton );
		buttonsLayout->addWidget( _gitButton );
		buttonsLayout->addWidget( _docButton );
		buttonsLayout->addWidget( _bugReportButton );
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
		mainVerticalLayout->addSpacing( 10 );
		mainVerticalLayout->addWidget( libraryTitle );
		mainVerticalLayout->addWidget( libraryDesc );
		mainVerticalLayout->addSpacing( 10 );
		mainVerticalLayout->addWidget( iconAuthorsTitle );
		mainVerticalLayout->addWidget( iconAuthors );

		setLayout( mainVerticalLayout );
	}

	void InformationWidget::_setupSlots() {};
	void InformationWidget::localize()
	{
		// Qt translate (not use currently)
		// setWindowTitle( QCoreApplication::translate( "About", "About", nullptr ) );
		setWindowTitle( "About" );
	}

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
