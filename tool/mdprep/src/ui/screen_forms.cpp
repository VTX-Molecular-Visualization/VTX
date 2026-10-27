#include <QTimer>
#include <memory>
#include <qpushbutton.h>
#include <ui/qt/helper.hpp>
#include <util/event_hub.hpp>
#include <util/exceptions.hpp>
//
#include "tool/mdprep/gateway/engine_job_manager.hpp"
#include "tool/mdprep/gateway/form_data.hpp"
#include "tool/mdprep/gateway/shared.hpp"
#include "tool/mdprep/ui/input_checker.hpp"
//
#include "tool/mdprep/ui/form.hpp"
#include "tool/mdprep/ui/form_switch_button.hpp"
#include "tool/mdprep/ui/md_engine.hpp"
#include "tool/mdprep/ui/md_engine_field_placer.hpp"
#include "tool/mdprep/ui/md_engine_specific_field_placer.hpp"
#include "tool/mdprep/ui/preparation_timeline.hpp"
#include "tool/mdprep/ui/report.hpp"
//
#include "tool/mdprep/ui/form_advanced/form_advanced.hpp"
#include "tool/mdprep/ui/form_basic/form_basic.hpp"
#include <app/services.hpp>
//
#include "tool/mdprep/ui/screen_forms.hpp"

namespace VTX::Tool::Mdprep::ui
{

	ScreenForms::ScreenForms( QWidget * const p_parent, Gateway::MdParameters & p_data ) : _dataPtr( &p_data )
	{
		_setupUi( p_parent );
		_setupSlots();
	}

	void ScreenForms::_setupUi( QWidget * const p_parent )
	{
		if ( p_parent->layout() )
		{
			p_parent->layout();
		}
		VTX::UI::QT::Helper::makeQObject<QVBoxLayout>( p_parent );
		QWidget * mainWidget = VTX::UI::QT::Helper::makeQObject<QWidget>( p_parent );
		p_parent->layout()->addWidget( mainWidget );
		mainWidget->setContentsMargins( { 0, 0, 0, 0 } );

		QVBoxLayout * qLayoutWindow = VTX::UI::QT::Helper::makeQObject<QVBoxLayout>( mainWidget );
		qLayoutWindow->setContentsMargins( 0, 0, 0, 0 );
		qLayoutWindow->addSpacerItem( std::make_unique<QSpacerItem>( 0, 10 ).release() );

		QHBoxLayout * qLayoutCentering = VTX::UI::QT::Helper::makeChildLayout<QHBoxLayout>( *qLayoutWindow );

		// following content is meant to be moved eventually
		_w_mdEngine = VTX::UI::QT::Helper::makeQObject<QComboBox>( mainWidget );
		for ( const auto & it : VTX::Tool::Mdprep::ui::mdEngineStrings() )
		{
			_w_mdEngine->addItem( QString( it ) );
		}
		_w_mdEngine->setCurrentIndex( _mdEngineCurrentIdx );

		qLayoutCentering->addStretch( 1 );

		QFormLayout * qLayoutFormEngine = VTX::UI::QT::Helper::makeChildLayout<QFormLayout>( *qLayoutCentering );
		QLabel *	  qLabelMdEngine = VTX::UI::QT::Helper::makeQObject<QLabel>( "Choose your MD engine", mainWidget );
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

		QWidget *	  qWidgetSwitch = VTX::UI::QT::Helper::makeQObject<QWidget>( mainWidget );
		QVBoxLayout * qLayoutSwitch = VTX::UI::QT::Helper::makeQObject<QVBoxLayout>( qWidgetSwitch );
		qLayoutWindow->addWidget( qWidgetSwitch );

		qLayoutWindow->addSpacerItem( std::make_unique<QSpacerItem>( 0, 10 ).release() );
		_formContainer = VTX::UI::QT::Helper::makeQObject<QWidget>( mainWidget );
		qLayoutWindow->addWidget( _formContainer );

		_updateMdEngine( 0 ); // Before setting up a form, the report manager needs to know the mdEngine

		// By default we display the basic form
		_switchButton.subscribeBasicSwitch( [ & ] { this->_setFormBasic(); } );
		_switchButton.subscribeAdvancedSwitch( [ & ] { this->_setFormAdvanced(); } );
		_switchButton.setupUi( qLayoutSwitch, VTX::Tool::Mdprep::ui::FormSwitchButton::E_FORM_MODE::basic );

		QLabel *			qExplainatoryText = VTX::UI::QT::Helper::makeQObject<QLabel>( mainWidget );
		static const char * buttonLabel		  = "Prepare system";
		qExplainatoryText->setText(
			QString::fromUtf8(
				"Pushing the <i>%1</i> button will use every <b>visible</b> object(s) of the system to "
				"<b>prepare</b> a Molecular Dynamics Simulation using selected parameters.<br><u>Be wary :</u> <i>VTX "
				"doesn't support "
				"yet</i> automatic MD preparation for <b>small organic molecule</b>. Hence, any visible "
				"non-biological entity is likely cause preparation failure. Please mind the automatic check result." )
				.arg( buttonLabel ) );
		qExplainatoryText->setWordWrap( true );
		qExplainatoryText->setContentsMargins( { 10, 10, 5, 5 } );
		qLayoutWindow->addWidget( qExplainatoryText );
		_buttonStart = VTX::UI::QT::Helper::makeQObject<QPushButton>( mainWidget );
		QFont f		 = _buttonStart->font();
		f.setPointSize( f.pointSize() + 2 );
		_buttonStart->setFont( f );
		_buttonStart->setText( buttonLabel );
		qLayoutWindow->addWidget( _buttonStart );
	}

	void ScreenForms::_updateMdEngine( int idx )
	{
		_mdEngineCurrentIdx = idx;
		auto & mdEngine		= _mdEngines[ _mdEngineCurrentIdx ];
		if ( not mdEngine.has_value() )
		{
			mdEngine.emplace();
			VTX::Tool::Mdprep::ui::get( static_cast<VTX::Tool::Mdprep::ui::E_MD_ENGINE>( _mdEngineCurrentIdx ),
										mdEngine.value() );
		}
		{
			_formEngine = VTX::Tool::Mdprep::ui::MdEngineFieldPlacer();
			mdEngine->get( _formEngine );
			mdEngine->get( _jobManager );

			// The form doesn't actually know if the report ui must be relocated. Therefore it is handled here.
			ReportManager _newReportManager { _jobManager };
			if ( _reportManager.has_value() )
			{
				_newReportManager.relocate( _reportManager.value() );
			}
			_reportManager.emplace( std::move( _newReportManager ) );

			VTX::Tool::Mdprep::ui::FormLayouts layouts;
			_currentForm.get( layouts );
			_formEngine.assign( layouts );
			_formEngine.activate();
		}

		_updateForm();
	}

	std::function<VTX::Tool::Mdprep::ui::MdEngineSpecificFieldPlacer( const VTX::Tool::Mdprep::ui::E_FIELD_SECTION & )>
	ScreenForms::SpecificFieldPlacerGetter()
	{
		return [ & ]( const VTX::Tool::Mdprep::ui::E_FIELD_SECTION & p_section )
		{
			VTX::Tool::Mdprep::ui::MdEngineSpecificFieldPlacer p;
			_mdEngines[ _mdEngineCurrentIdx ]->get( p_section, p );
			return p;
		};
	}

	void ScreenForms::_updateForm()
	{
		const auto & mdEngine = _mdEngines[ _mdEngineCurrentIdx ];
		if ( mdEngine.has_value() )
		{
			VTX::Tool::Mdprep::ui::EngineSpecificCommonInformation engineSpecificData;
			VTX::Tool::Mdprep::ui::get( mdEngine.value(), engineSpecificData );
			_currentForm.update( engineSpecificData );
		}
	}

	void ScreenForms::_setFormBasic()
	{
		if ( not _reportManager )
		{
			throw VTXException( "Report manager is not initialized" );
		}
		ReportManager & reportManager = *_reportManager;
		_formEngine.deactivate();
		VTX::Tool::Mdprep::Gateway::MdParameters param;
		_currentForm.get( param );
		_currentForm.close();
		_currentForm = VTX::Tool::Mdprep::ui::form_basic::FormBasic(
			_formContainer, SpecificFieldPlacerGetter(), param, reportManager );

		VTX::Tool::Mdprep::ui::FormLayouts layouts;
		_currentForm.get( layouts );
		_formEngine.assign( layouts );
		_formEngine.activate();
		_updateForm();
	}

	void ScreenForms::_setFormAdvanced()
	{
		if ( not _reportManager )
		{
			throw VTXException( "Report manager is not initialized" );
		}
		ReportManager & reportManager = *_reportManager;
		_formEngine.deactivate();
		VTX::Tool::Mdprep::Gateway::MdParameters param;
		_currentForm.get( param );
		_currentForm.close();
		_currentForm = VTX::Tool::Mdprep::ui::form_advanced::FormAdvanced(
			_formContainer, SpecificFieldPlacerGetter(), param, reportManager );

		VTX::Tool::Mdprep::ui::FormLayouts layouts;
		_currentForm.get( layouts );
		_formEngine.assign( layouts );
		_formEngine.activate();
		_updateForm();
	}

	void ScreenForms::_startPreparation()
	{
		const auto & mdEngine = _mdEngines[ _mdEngineCurrentIdx ];
		if ( not mdEngine.has_value() )
		{
			return; // No MD engine selected/initialized yet : nothing to prepare.
		}

		Gateway::MdParameters param;
		_currentForm.get( param );

		// Open (and reset) the timeline dock so the user can follow the job step by step. It listens to the
		// preparation events fired from prepareStructure / the packing step.
		showPreparationTimeline();

		_jobManager.startPreparation( param );
		_buttonStart->setDisabled( true );
	}

	void ScreenForms::_preparationEnd()
	{
		QTimer::singleShot( 0, _buttonStart, [ & ] { _buttonStart->setDisabled( false ); } );
	}

	void ScreenForms::_setupSlots()
	{
		QObject::connect( _w_mdEngine,
						  &QComboBox::currentIndexChanged,
						  [ & ]( int p_newIdx ) { this->_updateMdEngine( p_newIdx ); } );
		QObject::connect( _buttonStart, &QPushButton::clicked, [ & ]() { this->_startPreparation(); } );
		_preparationEndConnection
			= App::HUB().connect<Gateway::PreparationFinished, &ScreenForms::_preparationEnd>( this );
	}
} // namespace VTX::Tool::Mdprep::ui
