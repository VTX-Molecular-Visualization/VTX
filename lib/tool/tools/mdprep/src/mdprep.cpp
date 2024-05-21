#include "tools/mdprep/mdprep.hpp"
#include "tools/mdprep/ui/md_engine_form.hpp"
//
#include "tools/mdprep/ui/basic_form.hpp"
#include "tools/mdprep/ui/form_data.hpp"
#include "tools/mdprep/ui/main_window.hpp"
#include <qcombobox.h>
#include <qformlayout.h>
#include <qlineedit.h>
#include <ui/qt/application_qt.hpp>
#include <ui/qt/main_window.hpp>
#include <util/logger.hpp>

namespace VTX::QT::Mdprep
{
	// Class responsible for managing the mdprep main window by coordinating the common form and the md engine
	// specifics.
	class MainWindow : public UI::QT::QtDockablePanel
	{
		using FormCollection
			= std::array<std::optional<VTX::Tool::Mdprep::ui::MdEngineForm>, VTX::Tool::Mdprep::ui::MD_ENGINE_NUMBER>;

		inline static const QSize PREFERRED_SIZE { 640, 720 };
		QComboBox *				  _w_mdEngine = nullptr;

		VTX::Tool::Mdprep::ui::MdFieldsOrganizer _fieldOrganizer;
		VTX::Tool::Mdprep::ui::MdBasicParamForm	 _formBasic;
		FormCollection							 _formsMd;
		int										 _mdEngineCurrentIdx = 0;

		// Problem : we must support multiple engines with some common fields and some different ones.
		// I need to find a way to update the form dynamically whilst keeping common field (such as equilibration time)
		// untouched. We separate fields in two categories : Base settings and Advanced Settings. In both of those
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

			// following content is meant to be moved eventually
			_w_mdEngine = new QComboBox;
			for ( auto & it : VTX::Tool::Mdprep::ui::mdEngineStrings() )
				_w_mdEngine->addItem( QString( it ) );
			_w_mdEngine->setCurrentIndex( _mdEngineCurrentIdx );
			windowLayout->addWidget( _w_mdEngine, 1 );

			_fieldOrganizer.setupUi( windowLayout );
		}
		void _updateFormEngine( int idx ) noexcept
		{
			if ( _formsMd[ _mdEngineCurrentIdx ].has_value() )
				_formsMd[ _mdEngineCurrentIdx ]->deactivate();

			_mdEngineCurrentIdx = idx;

			if ( _formsMd[ _mdEngineCurrentIdx ].has_value() == false )
				_formsMd[ _mdEngineCurrentIdx ] = VTX::Tool::Mdprep::ui::form(
					static_cast<VTX::Tool::Mdprep::ui ::E_MD_ENGINE>( _mdEngineCurrentIdx ), _fieldOrganizer.layouts
				);
			_formsMd[ _mdEngineCurrentIdx ]->activate();
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
			_updateFormEngine( 0 );
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
