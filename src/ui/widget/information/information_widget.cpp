#include "information_widget.hpp"
#include "action/action_manager.hpp"
#include "action/main.hpp"
#include "define.hpp"
#include "style.hpp"
#include "util/filesystem.hpp"
#include <QDesktopServices>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <string>

namespace VTX::UI::Widget::Information
{
	InformationWidget::InformationWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {}

	InformationWidget ::~InformationWidget()
	{
		if ( _movie != nullptr )
			delete _movie;
	}

	void InformationWidget::showEvent( QShowEvent * p_event )
	{
		if ( _movie != nullptr )
		{
			_movie->stop();
			_movie->start();
		}
	}

	void InformationWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		setWindowFlag( Qt::WindowType::WindowContextHelpButtonHint, false );

		QLabel * const vtxLogo = new QLabel( this );

		if ( Style::VIDEO_IN_INFORMATION )
		{
			_movie = new QMovie( this );
			_movie->setFileName( ":/video/video_intro.gif" );
			vtxLogo->setFixedSize( QSize( 131, 143 ) );
			vtxLogo->setAutoFillBackground( true );
			vtxLogo->setMovie( _movie );
		}
		else
		{
			QPixmap logo = QPixmap( ":/sprite/logo.png", nullptr, Qt::ImageConversionFlag::ColorOnly );
			vtxLogo->setPixmap( logo );
			vtxLogo->setScaledContents( true );
			vtxLogo->setFixedSize( QSize( 64, 64 ) );
		}

		vtxLogo->setScaledContents( true );

		QLabel * versionLabel = new QLabel( this );
		versionLabel->setText( "Version: " + _getVersionText() );

		QLabel * licenseTitle = new QLabel( this );
		licenseTitle->setText( "License:" );
		QScrollArea * const licenseScrollArea = new QScrollArea( this );
		QLabel * const		licenseTxt		  = new QLabel( this );
		QString				licenseStr;
		_getLicenseText( licenseStr );
		licenseTxt->setText( licenseStr );
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
		_checkUpdateButton = new QPushButton( this );
		_checkUpdateButton->setText( "Update" );

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
			"<a href =\"https://chemfiles.org/\">Chemfiles</a> - "
			"<a href =\"https://eigen.tuxfamily.org/\">Eigen</a> - "
			"<a href =\"https://fmt.dev/latest/index.html\">fmt</a> - "
			"<a href =\"https://www.opengl.org/sdk/libs/GLM/\">GLM</a> - "
			"<a href =\"https://github.com/Neargye/magic_enum\">Magic enum</a> - "
			"<a href =\"https://github.com/nlohmann/json\">nlohmann</a> - "
			"<a href = \"https://www.qt.io/\">Qt</a>"
			"" );

		QLabel * iconAuthorsTitle = new QLabel( this );
		iconAuthorsTitle->setText( "Icons authors:" );
		QLabel * iconAuthors = new QLabel( this );
		iconAuthors->setTextInteractionFlags( libraryDesc->textInteractionFlags()
											  | Qt::TextInteractionFlag::LinksAccessibleByMouse );
		iconAuthors->setOpenExternalLinks( true );
		iconAuthors->setWordWrap( true );
		iconAuthors->setFrameShape( QFrame::Shape::Panel );
		iconAuthors->setFrameShadow( QFrame::Shadow::Sunken );
		iconAuthors->setText(
			"<a href = \"https://www.flaticon.com/authors/alfredo-hernandez\">alfredo-hernandez</a> - "
			"<a href = \"https://www.flaticon.com/authors/Becris\">Becris</a> - "
			"<a href = \"https://www.flaticon.com/authors/dinosoftlabs\">dinosoftlabs</a> - "
			"<a href = \"https://www.flaticon.com/authors/freepik\">Freepik</a> - "
			"<a href = \"https://www.flaticon.com/authors/good-ware\">good-ware</a> - "
			"<a href = \"https://www.flaticon.com/authors/google\">google</a> - "
			"<a href = \"https://www.flaticon.com/authors/itim2101\">itim2101</a> - "
			"<a href = \"https://www.flaticon.com/authors/kiranshastry\">kiranshastry</a> - "
			"<a href = \"https://www.flaticon.com/authors/mavadee\">mavadee</a> - "
			"<a href = \"https://www.flaticon.com/authors/pixel-perfect\">pixel-perfect</a> - "
			"<a href = \"https://www.flaticon.com/authors/roundicons\">roundicons</a> - "
			"<a href = \"https://www.flaticon.com/authors/tanah-basah\">Tanah Basah</a> - "
			"<a href = \"https://www.flaticon.com/authors/tempo-doloe\">tempo-doloe</a> - "
			"<a href = \"https://www.flaticon.com/authors/vectors-market\">vectors-market</a>"
			"" );

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
		buttonsLayout->addWidget( _checkUpdateButton );
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

	void InformationWidget::_setupSlots()
	{
		connect( _movie, &QMovie::frameChanged, this, &InformationWidget::_onFrameChange );
		connect( _movie, &QMovie::error, this, &InformationWidget::_displayMovieError );

		connect( _websiteButton, &QPushButton::clicked, this, &InformationWidget::_goToWebsite );
		connect( _gitButton, &QPushButton::clicked, this, &InformationWidget::_goToGit );
		connect( _docButton, &QPushButton::clicked, this, &InformationWidget::_goToDocumentation );
		connect( _bugReportButton, &QPushButton::clicked, this, &InformationWidget::_goToBugReport );
		connect( _checkUpdateButton, &QPushButton::clicked, this, &InformationWidget::_checkForUpdate );
	};
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

	void InformationWidget::_getLicenseText( QString & p_txt ) const
	{
		Util::Filesystem::readPathQString( Util::Filesystem::getLicenseFile(), p_txt );
	}

	void InformationWidget::_onFrameChange( const int p_frame )
	{
		_movie->setPaused( p_frame == _movie->frameCount() - 1 );
	}
	void InformationWidget::_displayMovieError( QImageReader::ImageReaderError p_error ) const
	{
		VTX_ERROR( "Error when loading video : " + std::to_string( int( p_error ) ) );
	}

	void InformationWidget::_goToWebsite() const
	{
		QDesktopServices::openUrl( QString::fromStdString( VTX_WEBSITE_URL ) );
	}
	void InformationWidget::_goToGit() const { QDesktopServices::openUrl( QString::fromStdString( VTX_GIT_URL ) ); }
	void InformationWidget::_goToDocumentation() const
	{
		QDesktopServices::openUrl( QString::fromStdString( VTX_DOCUMENTATION_URL ) );
	}
	void InformationWidget::_goToBugReport() const
	{
		QDesktopServices::openUrl( QString::fromStdString( VTX_BUG_REPORT_URL ) );
	}
	void InformationWidget::_checkForUpdate() const { VTX_ACTION( new Action::Main::CheckForUpdate( true ) ); }

} // namespace VTX::UI::Widget::Information
