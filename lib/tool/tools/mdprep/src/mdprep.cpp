#include "tools/mdprep/mdprep.hpp"
#include "tools/mdprep/ui/main_window.hpp"
#include <qcombobox.h>
#include <qformlayout.h>
#include <qlineedit.h>
#include <ui/qt/application_qt.hpp>
#include <ui/qt/main_window.hpp>
#include <util/logger.hpp>

namespace VTX::QT::Mdprep
{
	class MainWindow : public UI::QT::QtDockablePanel
	{
	  private:
		inline static const QSize PREFERRED_SIZE { 640, 720 };
		virtual void			  _setupUi( const QString & p_name )
		{
			auto		_t = p_name.toLatin1();
			std::string v( _t.begin(), _t.end() );
			VTX::VTX_INFO( "info from Mdprep::MainWindow::_setupUi : <{}>", v );
			QWidget * const mainWidget = _instantiateMainWidget( PREFERRED_SIZE, PREFERRED_SIZE );

			UI::QT::QtDockablePanel::_setupUi( p_name );

			this->setWindowTitle( "Molecular Dynamics Preparation" );

			setWindowState( Qt::WindowState::WindowActive );
			const QSize winsize = QSize( 640, 720 );
			resize( winsize );

			QVBoxLayout * windowLayout = new QVBoxLayout( mainWidget );
			windowLayout->setContentsMargins( 0, 0, 0, 0 );

			QComboBox * engineList = new QComboBox;
			for ( auto & it : VTX::Tool::Mdprep::ui::mdEngineStrings() )
				engineList->addItem( QString( it ) );

			QWidget * mainContainer = new QWidget( this );
			mainContainer->setStyleSheet( "border: 1px solid red" );
			mainContainer->setSizePolicy( QSizePolicy( QSizePolicy ::Expanding, QSizePolicy ::Expanding ) );
			windowLayout->addWidget( mainContainer, 1 );
			QHBoxLayout * hLayout = new QHBoxLayout( mainContainer );

			auto layoutToAddThoseTextboxTo = hLayout;

			QLineEdit * textbox = new QLineEdit();
			textbox->setText( "textbox" );
			// QFormLayout * layout = new QFormLayout( this );
			// layout->addRow( tr( "&Textbox:" ), textbox );
			// layout->addRow( tr( "&Textbox2:" ), textbox2 );
			// hLayout->addLayout( layout );
			layoutToAddThoseTextboxTo->addWidget( engineList, 1 );
			layoutToAddThoseTextboxTo->addWidget( textbox, 1 );
		}
		virtual void _setupSlots() { VTX::VTX_INFO( "info from Mdprep::MainWindow::_setupSlots" ); }

	  public:
		MainWindow( QWidget * const p_parent = nullptr ) : UI::QT::QtDockablePanel( p_parent )
		{
			_setupUi( "Is this parameter useful ?" );
		}
	};
} // namespace VTX::QT::Mdprep

namespace VTX::Tool::Mdprep
{

	class MainWindow::_impl
	{
		VTX::QT::Mdprep::MainWindow _win { &VTX::UI::QT::QT_APP()->getMainWindow() };

	  public:
		_impl() {}
		void show() noexcept { _win.show(); }
	};
	MainWindow::MainWindow() : _pimpl( new MainWindow::_impl() ) {}

	// Assumes pimpl is always valid ptr
	void MainWindow::show() noexcept { _pimpl->show(); }
	void MainWindow::Del::operator()( _impl * p_ ) noexcept { delete p_; }
} // namespace VTX::Tool::Mdprep
