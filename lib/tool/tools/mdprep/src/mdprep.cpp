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
		inline static const QSize PREFERRED_SIZE { 640, 720 };

		QComboBox * _w_mdEngine			= nullptr;
		int			_mdEngineCurrentIdx = 0;

		std::vector<std::string> _forcefieldCollection;

		// Problem : we must support multiple engines with some common fields and some different ones.
		// I need to find a way to update the form dynamically whilst keeping common field (such as equilibration time)
		// untouched We separate fields in two categories : Base settings and Advanced Settings. In both of those
		// categories, it will be common field, and engine-specific fields. I should instanciate an object that frame
		// engine-specific fields. Don't forget that we need to connect and disconnect events on change. So the object
		// framing the engine-specific behavior shall be responsible of this as well.

		virtual void _setupUi( const QString & p_name )
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

			_w_mdEngine = new QComboBox;
			for ( auto & it : VTX::Tool::Mdprep::ui::mdEngineStrings() )
				_w_mdEngine->addItem( QString( it ) );
			_w_mdEngine->setCurrentIndex( _mdEngineCurrentIdx );
			windowLayout->addWidget( _w_mdEngine, 1 );
		}
		void _updateFormEngine( int idx ) noexcept
		{
			_mdEngineCurrentIdx = idx;
			VTX::VTX_DEBUG( "info from Mdprep::MainWindow::_updateFormEngine({})", idx );
		}
		virtual void _setupSlots()
		{
			VTX::VTX_INFO( "info from Mdprep::MainWindow::_setupSlots" );
			connect( _w_mdEngine, &QComboBox::currentIndexChanged, this, &MainWindow ::_updateFormEngine );
		}

	  public:
		MainWindow( QWidget * const p_parent = nullptr ) : UI::QT::QtDockablePanel( p_parent )
		{
			_setupUi( "Is this parameter useful ?" );
			_setupSlots();
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
