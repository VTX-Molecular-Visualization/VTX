
#include "util/sentry.hpp"
#include <qpushbutton.h>
#include <ui/qt/util.hpp>
//
#include "tools/mdprep/gateway/engine_job_manager.hpp"
#include "tools/mdprep/gateway/form_data.hpp"
#include "tools/mdprep/gateway/shared.hpp"
//
#include "tools/mdprep/ui/form.hpp"
#include "tools/mdprep/ui/form_switch_button.hpp"
#include "tools/mdprep/ui/input_checker.hpp"
#include "tools/mdprep/ui/md_engine.hpp"
#include "tools/mdprep/ui/md_engine_field_placer.hpp"
#include "tools/mdprep/ui/md_engine_specific_field_placer.hpp"
#include "tools/mdprep/ui/report.hpp"
//
#include "tools/mdprep/ui/form_advanced/form_advanced.hpp"
#include "tools/mdprep/ui/form_basic/form_basic.hpp"
//
#include "tools/mdprep/ui/screen_forms.hpp"

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

		_switchButton.setupUi( qLayoutWindow, VTX::Tool::Mdprep::ui::FormSwitchButton::E_FORM_MODE::basic );

		qLayoutWindow->addSpacerItem( new QSpacerItem( 0, 10 ) );
		_formContainer = new QWidget;
		qLayoutWindow->addWidget( _formContainer );

		_updateMdEngine( 0 ); // Before setting up a form, the report manager needs to know the mdEngine

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
			_reportManager.emplace( _jobManager );
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
	void ScreenForms::_setupSlots() noexcept
	{
		QObject::connect(
			_w_mdEngine, &QComboBox::currentIndexChanged, [ & ]( int p_newIdx ) { this->_updateMdEngine( p_newIdx ); }
		);
		_switchButton.subscribeBasicSwitch( [ & ] { this->_setFormBasic(); } );
		_switchButton.subscribeAdvancedSwitch( [ & ] { this->_setFormAdvanced(); } );
	}

} // namespace VTX::Tool::Mdprep::ui
