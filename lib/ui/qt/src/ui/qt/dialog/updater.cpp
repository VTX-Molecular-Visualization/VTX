#include "ui/qt/dialog/updater.hpp"
#include <QDialogButtonBox>
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
		auto * labelTitle = new QLabel( QString( "Version %1 available" ).arg( p_e.newVersion ) );
		labelTitle->setContentsMargins( 0, 20, 0, 20 );
		QFont font = labelTitle->font();
		font.setPointSizeF( font.pointSizeF() * 2 );
		labelTitle->setFont( font );
		labelTitle->setAlignment( Qt::AlignCenter );
		layout->addWidget( labelTitle );

		// Changelog.
		auto * browser = new QTextBrowser( this );
		browser->document()->setDocumentMargin( 10 );
		browser->setHtml( QString::fromStdString( p_e.changelogHtml ) );
		layout->addWidget( browser );

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

		// Buttons.
		auto * buttonBox = new QDialogButtonBox(
			QDialogButtonBox::StandardButton::Close | QDialogButtonBox::StandardButton::Apply, this
		);
		layout->addWidget( buttonBox );

		// Connect.
		connect( buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept );
		connect( buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject );

		connect(
			this, &QDialog::accepted, [ this, p_e ]() { App::ACTION().execute<App::Action::Application::Update>(); }
		);
	}

} // namespace VTX::UI::QT::Dialog
