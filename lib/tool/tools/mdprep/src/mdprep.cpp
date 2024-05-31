#include <qcombobox.h>
#include <qformlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
//
#include "tools/mdprep/mdprep.hpp"
#include "tools/mdprep/ui/md_engine_form.hpp"
//
#include "tools/mdprep/ui/form_data.hpp"
#include "tools/mdprep/ui/main_window.hpp"
#include <ui/qt/application_qt.hpp>
#include <ui/qt/main_window.hpp>
#include <ui/qt/widget_factory.hpp>
#include <util/logger.hpp>
//
#include "tools/mdprep/ui/advanced_form.hpp"
#include "tools/mdprep/ui/basic_form.hpp"
//

namespace VTX::QT::Mdprep
{

	// Class responsible for managing the mdprep main window by coordinating the common form and the md engine
	// specifics.
	class MainWindow : public UI::QT::QtDockablePanel
	{
		using FormCollection
			= std::array<std::optional<VTX::Tool::Mdprep::ui::MdEngineForm>, VTX::Tool::Mdprep::ui::MD_ENGINE_NUMBER>;

		inline static const QSize PREFERRED_SIZE { 500, 720 };
		QComboBox *				  _w_mdEngine = nullptr;

		VTX::Tool::Mdprep::ui::MdFieldsOrganizer   _fieldOrganizer;
		VTX::Tool::Mdprep::ui::MdBasicParamForm	   _formBasic;
		VTX::Tool::Mdprep::ui::MdAdvancedParamForm _formAdvanced;
		FormCollection							   _formsMd;
		int										   _mdEngineCurrentIdx = 0;

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
			this->setWindowIcon( QIcon( ":/sprite/icon_tool_mdprep_mainButton.png" ) );
			this->setWindowTitle( "Molecular Dynamics Preparation" );

			setWindowState( Qt::WindowState::WindowActive );
			const QSize winsize = PREFERRED_SIZE;
			resize( winsize );

			QVBoxLayout * qLayoutWindow = new QVBoxLayout( mainWidget );
			qLayoutWindow->setContentsMargins( 0, 0, 0, 0 );
			qLayoutWindow->addSpacerItem( new QSpacerItem( 0, 10 ) );

			QHBoxLayout * qLayoutCentering = new QHBoxLayout;
			qLayoutWindow->addLayout( qLayoutCentering );

			// following content is meant to be moved eventually
			_w_mdEngine = new QComboBox;
			for ( auto & it : VTX::Tool::Mdprep::ui::mdEngineStrings() )
				_w_mdEngine->addItem( QString( it ) );
			_w_mdEngine->setCurrentIndex( _mdEngineCurrentIdx );

			qLayoutCentering->addStretch( 1 );

			QFormLayout * qLayoutFormEngine = new QFormLayout;
			qLayoutCentering->addLayout( qLayoutFormEngine );
			QLabel * qLabelMdEngine = new QLabel( "Choose your MD engine" );
			{
				auto font = qLabelMdEngine->font();
				font.setPointSize( font.pointSize() + 2 );
				qLabelMdEngine->setFont( font );
				font = _w_mdEngine->font();
				font.setPointSize( font.pointSize() + 2 );
				_w_mdEngine->setFont( font );
			}
			qLayoutFormEngine->addRow( qLabelMdEngine, _w_mdEngine );

			qLayoutCentering->addStretch( 1 );

			qLayoutWindow->addSpacerItem( new QSpacerItem( 0, 10 ) );

			_fieldOrganizer.setupUi( qLayoutWindow, VTX::Tool::Mdprep::ui::MdFieldsOrganizer::E_FORM_MODE::basic );
			_formBasic.setupUi( _fieldOrganizer.containerParamBasic );
			_formAdvanced.setupUi( _fieldOrganizer.containerParamAdvanced );
			qLayoutWindow->addWidget( new QPushButton( "Button" ) );
		}
		void _updateFormEngine( int idx ) noexcept
		{
			if ( _formsMd[ _mdEngineCurrentIdx ].has_value() )
				_formsMd[ _mdEngineCurrentIdx ]->deactivate();

			_mdEngineCurrentIdx = idx;

			if ( _formsMd[ _mdEngineCurrentIdx ].has_value() == false )
				_formsMd[ _mdEngineCurrentIdx ] = VTX::Tool::Mdprep::ui::form(
					static_cast<VTX::Tool::Mdprep::ui ::E_MD_ENGINE>( _mdEngineCurrentIdx ),
					{ _formBasic.layoutFieldsMdEngine(), _formBasic.layoutFieldsMdEngine() }
				);
			_formsMd[ _mdEngineCurrentIdx ]->activate();

			{
				const VTX::Tool::Mdprep::ui::EngineSpecificCommonFormData * engineSpecificData = nullptr;
				_formsMd[ _mdEngineCurrentIdx ]->get( engineSpecificData );
				if ( engineSpecificData )
					_formBasic.update( *engineSpecificData );
			}

			VTX::VTX_DEBUG( "info from Mdprep::MainWindow::_updateFormEngine({})", idx );
		}
		virtual void _setupSlots()
		{
			VTX::VTX_INFO( "info from Mdprep::MainWindow::_setupSlots" );
			connect( _w_mdEngine, &QComboBox::currentIndexChanged, this, &MainWindow ::_updateFormEngine );
			_formAdvanced.subscribe( [ & ]( const VTX::Tool::Mdprep::ui::MdAdvancedDataSample & p_data )
									 { _formBasic.update( p_data ); } );
			_formBasic.subscribe( [ & ]( const VTX::Tool::Mdprep::ui::MdBasicDataSample & p_data )
								  { _formAdvanced.update( p_data ); } );
		}

	  public:
		MainWindow( QWidget * const p_parent ) : UI::QT::QtDockablePanel( p_parent )
		// MainWindow( QWidget * const p_parent, const std::string & p_name ) : UI::QT::QtDockablePanel( p_parent )
		{
			_setupUi( QString::fromStdString( "name" ) );
			_updateFormEngine( 0 );
			_setupSlots();
		}
	};
} // namespace VTX::QT::Mdprep

namespace VTX::Tool::Mdprep
{

	class MainWindow::_impl
	{
		VTX::QT::Mdprep::MainWindow * _win
			= VTX::UI::QT::WidgetFactory::get().instantiateWidget<VTX::QT::Mdprep::MainWindow>(
				reinterpret_cast<QWidget *>( &VTX::UI::QT::QT_APP()->getMainWindow() ),
				"MdPrep Tool"
			);

	  public:
		_impl() {}
		void show() noexcept { _win->show(); }
	};
	MainWindow::MainWindow() : _pimpl( new MainWindow::_impl() ) {}

	// Assumes pimpl is always valid ptr
	void MainWindow::show() noexcept { _pimpl->show(); }
	void MainWindow::Del::operator()( _impl * p_ ) noexcept { delete p_; }
} // namespace VTX::Tool::Mdprep
