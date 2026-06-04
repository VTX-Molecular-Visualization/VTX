#include "ui/qt/dialog/updater.hpp"
#include <QDialogButtonBox>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <app/action/action_manager.hpp>
#include <app/action/application.hpp>
#include <app/services.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::Dialog
{

	Updater::Updater( const App::Events::UpdateAvailable & p_e )
	{
		setWindowTitle( "Updater" );
		setFixedSize( 800, 600 );

		// Layout.
		auto * layout = new QVBoxLayout( this );
		setLayout( layout );

		// Title.
		auto * labelTitle = new QLabel( QString( "VTX %1 available" ).arg( p_e.newVersion ) );
		labelTitle->setContentsMargins( 0, 20, 0, 20 );
		QFont font = labelTitle->font();
		font.setPointSizeF( font.pointSizeF() * 2 );
		labelTitle->setFont( font );
		labelTitle->setAlignment( Qt::AlignCenter );
		layout->addWidget( labelTitle );

		// Changelog.
		auto * browser = new QTextBrowser( this );
		browser->document()->setDocumentMargin( 10 );
		browser->setHtml( QString::fromStdString( p_e.changelog ) );
		layout->addWidget( browser );

		VTX_DEBUG( "Changelog: {}", p_e.changelog );

		// Infos.
		auto * labelCurrentVersion = new QLabel( QString( "Current: %1" ).arg( p_e.currentVersion ) );
		labelCurrentVersion->setAlignment( Qt::AlignRight );
		layout->addWidget( labelCurrentVersion );
		auto * labelLastVersion = new QLabel( QString( "New: %1" ).arg( p_e.newVersion ) );
		labelLastVersion->setAlignment( Qt::AlignRight );
		layout->addWidget( labelLastVersion );
		auto * labelSize = new QLabel( QString( "Size: %1" ).arg( Util::String::memSizeToStr( p_e.size ) ) );
		labelSize->setAlignment( Qt::AlignRight );
		layout->addWidget( labelSize );

		_progressBar = new QProgressBar( this );
		_progressBar->setRange( 0, 100 );
		_progressBar->setValue( 0 );
		_progressBar->setTextVisible( true );
		_progressBar->setFormat( "Downloading update... %p%" );
		layout->addWidget( _progressBar );

		_buttonBox = new QDialogButtonBox(
			QDialogButtonBox::StandardButton::Close | QDialogButtonBox::StandardButton::Apply, this
		);
		layout->addWidget( _buttonBox );
		QPushButton * const applyButton = _buttonBox->button( QDialogButtonBox::Apply );

		_restartButton = new QPushButton( "Restart", this );
		layout->addWidget( _restartButton );

		connect( _buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject );
		applyButton->setDefault( true );
		connect(
			applyButton,
			&QPushButton::clicked,
			this,
			[ this ]()
			{
				_setDownloadingState();
				App::ACTION().execute<App::Action::Application::Update>();
			}
		);
		connect(
			_restartButton,
			&QPushButton::clicked,
			this,
			[ this ]()
			{
				accept();
				App::ACTION().execute<App::Action::Application::RestartAfterUpdate>();
			}
		);

		App::HUB().connect<App::Events::UpdateDownloadProgress, &Updater::_onUpdateDownloadProgress>( this );
		App::HUB().connect<App::Events::UpdateReadyToRestart, &Updater::_onUpdateReadyToRestart>( this );
		App::HUB().connect<App::Events::UpdateDownloadFailed, &Updater::_onUpdateDownloadFailed>( this );

		_setIdleState();
	}

	Updater::~Updater() { App::HUB().disconnectAllOf( *this ); }

	void Updater::_onUpdateDownloadProgress( const App::Events::UpdateDownloadProgress & p_e )
	{
		if ( _progressBar )
		{
			_progressBar->setValue( int( p_e.progress ) );
		}
	}

	void Updater::_onUpdateReadyToRestart( const App::Events::UpdateReadyToRestart & ) { _setReadyToRestartState(); }

	void Updater::_onUpdateDownloadFailed( const App::Events::UpdateDownloadFailed & ) { _setIdleState(); }

	void Updater::_setDownloadingState()
	{
		_progressBar->setValue( 0 );
		_progressBar->show();
		_buttonBox->hide();
		_restartButton->hide();
	}

	void Updater::_setReadyToRestartState()
	{
		_progressBar->hide();
		_buttonBox->hide();
		_restartButton->show();
		_restartButton->setDefault( true );
		_restartButton->setFocus();
	}

	void Updater::_setIdleState()
	{
		_progressBar->hide();
		_buttonBox->show();
		_restartButton->hide();
	}

} // namespace VTX::UI::QT::Dialog
