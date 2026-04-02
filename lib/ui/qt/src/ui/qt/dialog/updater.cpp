#include "ui/qt/dialog/updater.hpp"
#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <app/action/action_manager.hpp>
#include <app/action/application.hpp>
#include <app/services.hpp>

namespace VTX::UI::QT::Dialog
{

	Updater::Updater( const App::Events::UpdateAvailable & p_e )
	{
		setWindowTitle( "Updater" );
		setFixedSize( 600, 600 );

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
		// browser->setHtml( QString::fromStdString( p_e.changelogHtml ) );
		browser->setText( QString::fromStdString( p_e.changelog ) );
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

		auto * labelStatus = new QLabel( this );
		labelStatus->setAlignment( Qt::AlignCenter );
		labelStatus->hide();
		layout->addWidget( labelStatus );

		// Buttons.
		auto * buttonBox = new QDialogButtonBox(
			QDialogButtonBox::StandardButton::Close | QDialogButtonBox::StandardButton::Apply, this
		);
		layout->addWidget( buttonBox );
		QPushButton * const applyButton = buttonBox->button( QDialogButtonBox::Apply );
		QPushButton * const closeButton = buttonBox->button( QDialogButtonBox::Close );

		// Connect.
		connect( buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject );
		applyButton->setDefault( true );
		connect(
			applyButton,
			&QPushButton::clicked,
			this,
			[ this, applyButton, closeButton, labelStatus ]()
			{
				applyButton->setEnabled( false );
				closeButton->setEnabled( false );
				labelStatus->setText( "Downloading update..." );
				labelStatus->show();
				App::ACTION().execute<App::Action::Application::Update>();
			}
		);
	}

} // namespace VTX::UI::QT::Dialog
