
#include "util/sentry.hpp"
#include <qpushbutton.h>
#include <ui/qt/util.hpp>
//
#include "tool/mdprep/gateway/engine_job_manager.hpp"
#include "tool/mdprep/gateway/form_data.hpp"
#include "tool/mdprep/gateway/shared.hpp"
//
#include "tool/mdprep/ui/form.hpp"
#include "tool/mdprep/ui/form_switch_button.hpp"
#include "tool/mdprep/ui/input_checker.hpp"
#include "tool/mdprep/ui/md_engine.hpp"
#include "tool/mdprep/ui/md_engine_field_placer.hpp"
#include "tool/mdprep/ui/md_engine_specific_field_placer.hpp"
#include "tool/mdprep/ui/report.hpp"
//
#include "tool/mdprep/ui/form_advanced/form_advanced.hpp"
#include "tool/mdprep/ui/form_basic/form_basic.hpp"
//
#include "tool/mdprep/ui/screen_forms.hpp"

namespace VTX::Tool::Mdprep::ui
{
	ScreenForms::ScreenForms( QWidget * p_parent, Gateway::MdParameters & p_data, ValidationSignaler p_validation ) :
		_dataPtr( &p_data ), _validationSignaler( std::move( p_validation ) )
	{
		_setupUi( p_parent );
		_setupSlots();
	}

	void ScreenForms::_setupUi( QWidget * p_parent ) noexcept
	{
		if ( p_parent->layout() )
			p_parent->layout();
		p_parent->setLayout( new QVBoxLayout );
		QWidget * mainWidget = new QWidget( p_parent );
		p_parent->layout()->addWidget( mainWidget );
		mainWidget->setContentsMargins( { 0, 0, 0, 0 } );

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

		QWidget *	  qWidgetSwitch = new QWidget;
		QVBoxLayout * qLayoutSwitch = new QVBoxLayout( qWidgetSwitch );
		qLayoutWindow->addWidget( qWidgetSwitch );

		qLayoutWindow->addSpacerItem( new QSpacerItem( 0, 10 ) );
		_formContainer = new QWidget;
		qLayoutWindow->addWidget( _formContainer );

		_updateMdEngine( 0 ); // Before setting up a form, the report manager needs to know the mdEngine

		// By default we display the basic form
		_switchButton.subscribeBasicSwitch( [ & ] { this->_setFormBasic(); } );
		_switchButton.subscribeAdvancedSwitch( [ & ] { this->_setFormAdvanced(); } );
		_switchButton.setupUi( qLayoutSwitch, VTX::Tool::Mdprep::ui::FormSwitchButton::E_FORM_MODE::basic );

		QLabel *			qExplainatoryText = new QLabel;
		static const char * buttonLabel		  = "Prepare system";
		qExplainatoryText->setText( QString::asprintf(
			"Pushing the <i>%s</i> button will use every <b>visible</b> object(s) of the system to "
			"<b>prepare</b> a Molecular Dynamics Simulation using selected parameters.<br><u>Be wary :</u> <i>VTX "
			"doesn't support "
			"yet</i> automatic MD preparation for <b>small organic molecule</b>. Hence, any visible "
			"non-biological entity is likely cause preparation failure. Please mind the automatic check result.",
			buttonLabel
		) );
		qExplainatoryText->setWordWrap( true );
		qExplainatoryText->setContentsMargins( { 10, 10, 5, 5 } );
		qLayoutWindow->addWidget( qExplainatoryText );
		_buttonStart = new QPushButton;
		QFont f		 = _buttonStart->font();
		f.setPointSize( f.pointSize() + 2 );
		_buttonStart->setFont( f );
		_buttonStart->setText( buttonLabel );
		qLayoutWindow->addWidget( _buttonStart );
	}
	void ScreenForms::_updateMdEngine( int idx ) noexcept
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
			_mdEngines[ _mdEngineCurrentIdx ]->get( _jobManager );

			// The form doesn't actually know if the report ui must be relocated. Therefore it is handled here.
			ReportManager _newReportManager { _jobManager };
			if ( _reportManager.has_value() )
				_newReportManager.relocate( _reportManager.value() );
			_reportManager.emplace( std::move( _newReportManager ) );

			VTX::Tool::Mdprep::ui::FormLayouts layouts;
			_currentForm.get( layouts );
			_formEngine.assign( std::move( layouts ) );
			_formEngine.activate();
		}

		_updateForm();
	}
	std::function<VTX::Tool::Mdprep::ui::MdEngineSpecificFieldPlacer( const VTX::Tool::Mdprep::ui::E_FIELD_SECTION & )>
	ScreenForms::_SpecificFieldPlacerGetter() noexcept
	{
		return [ & ]( const VTX::Tool::Mdprep::ui::E_FIELD_SECTION & p_section )
		{
			VTX::Tool::Mdprep::ui::MdEngineSpecificFieldPlacer p;
			_mdEngines[ _mdEngineCurrentIdx ]->get( p_section, p );
			return p;
		};
	}
	void ScreenForms::_updateForm() noexcept
	{
		if ( _mdEngines[ _mdEngineCurrentIdx ].has_value() )
		{
			VTX::Tool::Mdprep::ui::EngineSpecificCommonInformation engineSpecificData;
			VTX::Tool::Mdprep::ui::get( _mdEngines[ _mdEngineCurrentIdx ].value(), engineSpecificData );
			_currentForm.update( engineSpecificData );
		}
	}
	void ScreenForms::_setFormBasic() noexcept
	{
		_formEngine.deactivate();
		VTX::Tool::Mdprep::Gateway::MdParameters param;
		_currentForm.get( param );
		_currentForm.close();
		_currentForm = VTX::Tool::Mdprep::ui::form_basic::FormBasic(
			_formContainer, _SpecificFieldPlacerGetter(), param, *_reportManager
		);

		VTX::Tool::Mdprep::ui::FormLayouts layouts;
		_currentForm.get( layouts );
		_formEngine.assign( layouts );
		_formEngine.activate();
		_updateForm();
	}
	void ScreenForms::_setFormAdvanced() noexcept
	{
		_formEngine.deactivate();
		VTX::Tool::Mdprep::Gateway::MdParameters param;
		_currentForm.get( param );
		_currentForm.close();
		_currentForm = VTX::Tool::Mdprep::ui::form_advanced::FormAdvanced(
			_formContainer, _SpecificFieldPlacerGetter(), param, *_reportManager
		);

		VTX::Tool::Mdprep::ui::FormLayouts layouts;
		_currentForm.get( layouts );
		_formEngine.assign( layouts );
		_formEngine.activate();
		_updateForm();
	}
	void ScreenForms::_startPreparation() noexcept
	{
		Gateway::EngineJobManager jobManager;
		_mdEngines[ _mdEngineCurrentIdx ]->get( jobManager );

		Gateway::MdParameters param;
		_currentForm.get( param );

		Gateway::JobUpdateIntermediate intermediate;
		jobManager.startPreparation( param, intermediate.getUpdateCallback() );

		_validationSignaler.preparationStarted( std::move( intermediate ) );
	}
	void ScreenForms::_setupSlots() noexcept
	{
		QObject::connect(
			_w_mdEngine, &QComboBox::currentIndexChanged, [ & ]( int p_newIdx ) { this->_updateMdEngine( p_newIdx ); }
		);
		QObject::connect( _buttonStart, &QPushButton::clicked, [ & ]() { this->_startPreparation(); } );
	}

	ValidationSignaler::ValidationSignaler( std::function<void( Gateway::JobUpdateIntermediate )> p_ ) :
		_callback( std::move( p_ ) )
	{
	}

	void ValidationSignaler::preparationStarted( Gateway::JobUpdateIntermediate p_ ) noexcept
	{
		_callback( std::move( p_ ) );
	}

} // namespace VTX::Tool::Mdprep::ui
