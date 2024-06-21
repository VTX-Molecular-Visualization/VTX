#include <qcombobox.h>
#include <qformlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtextdocument.h>
//
#include "tools/mdprep/gateway/form_data.hpp"
#include "tools/mdprep/ui/shared.hpp"
//
#include "tools/mdprep/mdprep.hpp"
#include "tools/mdprep/ui/md_engine.hpp"
#include "tools/mdprep/ui/md_engine_factory.hpp"
#include "tools/mdprep/ui/md_engine_field_placer.hpp"
#include "tools/mdprep/ui/md_engine_specific_field_placer.hpp"
//
#include "tools/mdprep/ui/basic_form_settings_dialog.hpp"
#include "tools/mdprep/ui/md_field_organizer.hpp"
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
		using EngineCollection
			= std::array<std::optional<VTX::Tool::Mdprep::ui::MdEngine>, VTX::Tool::Mdprep::ui::MD_ENGINE_NUMBER>;

		inline static const QSize PREFERRED_SIZE { 500, 720 };
		QComboBox *				  _w_mdEngine	 = nullptr;
		QWidget *				  _formContainer = nullptr;

		// VTX::Tool::Mdprep::ui::FormSwitchButton   _fieldOrganizer;
		VTX::Tool::Mdprep::ui::FormSwitchButton					  _switchButton;
		std::optional<VTX::Tool::Mdprep::ui::MdBasicParamForm>	  _formBasic;
		std::optional<VTX::Tool::Mdprep::ui::MdAdvancedParamForm> _formAdvanced;
		EngineCollection										  _mdEngines;
		int														  _mdEngineCurrentIdx = 0;
		VTX::Tool::Mdprep::ui::MdEngineFieldPlacer				  _formEngine;

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

			_switchButton.setupUi( qLayoutWindow, VTX::Tool::Mdprep::ui::FormSwitchButton::E_FORM_MODE::basic );

			qLayoutWindow->addSpacerItem( new QSpacerItem( 0, 10 ) );
			_formContainer = new QWidget;
			qLayoutWindow->addWidget( _formContainer );

			// By default we display the basic form
			_setFormBasic();

			QLabel *			qExplainatoryText = new QLabel;
			static const char * buttonLabel		  = "Prepare system";
			qExplainatoryText->setText( QString::asprintf(
				"Pushing the <i>%s</i> button will use every <b>visible</b> object(s) of the system and attempts to "
				"<b>prepare</b> a Molecule Dynamic simulation from it.<br><u>Be wary :</u> <i>VTX doesn't support "
				"yet</i> automatic MD preparation for <b>small organic molecules</b>. Hence, any visible "
				"non-biological entity is likely cause preparation failure. Please mind the automatic check result.",
				buttonLabel
			) );
			qExplainatoryText->setWordWrap( true );
			qExplainatoryText->setContentsMargins( { 10, 10, 5, 5 } );
			qLayoutWindow->addWidget( qExplainatoryText );
			QPushButton * qStartButton = new QPushButton;
			QFont		  f			   = qStartButton->font();
			f.setPointSize( f.pointSize() + 2 );
			qStartButton->setFont( f );
			qStartButton->setText( buttonLabel );
			qLayoutWindow->addWidget( qStartButton );

			_updateMdEngine( 0 );
		}
		inline void _updateMdEngine( int idx ) noexcept
		{
			_mdEngineCurrentIdx = idx;
			if ( not _mdEngines[ _mdEngineCurrentIdx ].has_value() )
			{
				_mdEngines[ _mdEngineCurrentIdx ].emplace();
				VTX::Tool::Mdprep::ui::get(
					static_cast<VTX::Tool::Mdprep::ui::E_MD_ENGINE>( _mdEngineCurrentIdx ),
					_mdEngines[ _mdEngineCurrentIdx ].value()
				);
			}
			{
				_formEngine = VTX::Tool::Mdprep::ui::MdEngineFieldPlacer();
				_mdEngines[ _mdEngineCurrentIdx ]->get( _formEngine );
				VTX::Tool::Mdprep::ui::FormLayouts layouts;
				if ( _formAdvanced.has_value() )
					_formAdvanced->get( layouts );
				if ( _formBasic.has_value() )
					_formBasic->get( layouts );
				_formEngine.assign( std::move( layouts ) );
				_formEngine.activate();
			}

			_updateFormBasic();

			VTX::VTX_DEBUG( "info from Mdprep::MainWindow::_updateFormEngine({})", idx );
		}
		std::function<
			VTX::Tool::Mdprep::ui::MdEngineSpecificFieldPlacer( const VTX::Tool::Mdprep::ui::E_FIELD_SECTION & )>
		_SpecificFieldPlacerGetter() noexcept
		{
			return [ & ]( const VTX::Tool::Mdprep::ui::E_FIELD_SECTION & p_section )
			{
				VTX::Tool::Mdprep::ui::MdEngineSpecificFieldPlacer p;
				_mdEngines[ _mdEngineCurrentIdx ]->get( p_section, p );
				return p;
			};
		}
		inline void _updateFormBasic()
		{
			if ( _formBasic.has_value() && _mdEngines[ _mdEngineCurrentIdx ].has_value() )
			{
				VTX::Tool::Mdprep::ui::EngineSpecificCommonInformation engineSpecificData;
				VTX::Tool::Mdprep::ui::get( _mdEngines[ _mdEngineCurrentIdx ].value(), engineSpecificData );
				_formBasic->update( engineSpecificData );
			}
		}
		inline void _setFormBasic()
		{
			VTX::Tool::Mdprep::Gateway::MdParameters param;
			if ( _formAdvanced.has_value() )
			{
				_formAdvanced->get( param );
				_formAdvanced.reset();
			}
			_formBasic.emplace( _formContainer, _SpecificFieldPlacerGetter(), param );
			_updateFormBasic();
		}
		inline void _setFormAdvanced()
		{
			VTX::Tool::Mdprep::Gateway::MdParameters param;
			if ( _formBasic.has_value() )
			{
				_formBasic->get( param );
				_formBasic.reset();
			}
			_formAdvanced.emplace( _formContainer, param );

			// VTX::Tool::Mdprep::ui::EngineSpecificCommonInformation data;
			// VTX::Tool::Mdprep::ui::get( *_mdEngines[ _mdEngineCurrentIdx ], data );
			//_formAdvanced->update( data );
		}
		void _setupSlots()
		{
			VTX::VTX_INFO( "info from Mdprep::MainWindow::_setupSlots" );
			connect( _w_mdEngine, &QComboBox::currentIndexChanged, this, &MainWindow ::_updateMdEngine );
			_switchButton.subscribeBasicSwitch( [ & ] { this->_setFormBasic(); } );
			_switchButton.subscribeAdvancedSwitch( [ & ] { this->_setFormAdvanced(); } );
		}

	  public:
		MainWindow( QWidget * const p_parent ) : UI::QT::QtDockablePanel( p_parent ) {}
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
