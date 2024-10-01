#include <qformlayout.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qscrollarea.h>
#include <qscrollbar.h>
#include <qsizepolicy.h>
#include <qspinbox.h>
//
#include <ui/qt/util.hpp>
#include <util/logger.hpp>
#include <util/sentry.hpp>
//
#include "tool/mdprep/gateway/form_data.hpp"
#include "tool/mdprep/ui/input_checker.hpp"
#include "tool/mdprep/ui/md_engine_specific_field_placer.hpp"
#include "tool/mdprep/ui/report.hpp"
//
#include "tool/mdprep/ui/form_advanced/event_manager.hpp"
#include "tool/mdprep/ui/form_advanced/shared.hpp"
#include "tool/mdprep/ui/shared.hpp"
//
#include "tool/mdprep/ui/form_advanced/form_advanced.hpp"

namespace VTX::Tool::Mdprep::ui::form_advanced
{

	using namespace VTX::UI::QT::Util;
	using namespace VTX::Tool::Mdprep::Gateway;
	namespace
	{
		QDoubleSpinBox * addDoubleTextbox(
			QFormLayout *  p_dest,
			const char *   p_label,
			const char *   p_tooltip,
			const double & p_defaultValue,
			const double & p_step,
			const double & p_min,
			const double & p_max,
			const int &	   p_precision
		)
		{
			QDoubleSpinBox * out = new QDoubleSpinBox;
			out->setDecimals( p_precision );
			out->setSingleStep( p_step );
			out->setRange( p_min, p_max );
			out->setValue( p_defaultValue ); // set default here
			// p_dest->addRow( VTX::UI::QT::Util::createLabelWithHelpTooltip( p_label, p_tooltip ), out );
			return out;
		}
	} // namespace
	FormAdvanced::FormAdvanced(
		QWidget *					 p_container,
		SpecificFieldsPlacerCallback p_specificFieldGenerator,
		const MdParameters &		 p_defaults,
		ReportManager &				 p_reportManager
	) : _eventManager( p_reportManager, p_defaults ), _specificFieldGenerator( std::move( p_specificFieldGenerator ) )
	{
		_createAndPlaceUiItems( p_container );
		_loadValues( p_defaults );
		_eventManager.performFirstInputCheck( p_defaults );
	}
	namespace
	{
		template<typename T>
		inline void extractResultsFromFields( const T & p_fields, MdParametersStepData & p_out ) noexcept
		{
			p_out.dt = p_fields.dt->value();
			VTX::UI::QT::Util::get( p_fields.nsteps, p_out.nsteps );
			VTX::UI::QT::Util::get( p_fields.saveInterval, p_out.saveInterval );
		}
	} // namespace
	void FormAdvanced::get( MdParameters & p_out ) const noexcept
	{
		p_out.system.boxShapeIdx			= _uiObjects._fieldSystemBoxShape->currentIndex();
		p_out.system.forcefieldBio			= _uiObjects._fieldSystemBioForceField->currentText().toStdString();
		p_out.system.waterModel				= _uiObjects._fieldSystemWaterModel->currentText().toStdString();
		p_out.minimization.stepNumber		= _uiObjects._fieldMinimizationStepNumber->text().toUInt();
		p_out.minimization.stepSize			= _uiObjects._fieldMinimizationStepSize->value();
		p_out.minimization.energyBreakpoint = _uiObjects._fieldMinimizationStopThreshold->text().toDouble();
		extractResultsFromFields( _uiObjects._equilibrationNvt, p_out.nvt );
		extractResultsFromFields( _uiObjects._equilibrationNpt, p_out.npt );
		extractResultsFromFields( _uiObjects._production, p_out.prod );
		_fieldsIons.get( p_out.system );
		for ( auto & it_mdFieldPlacer : _eventManager.fieldPlacers )
			it_mdFieldPlacer.apply();
	}
	void FormAdvanced::get( FormLayouts & p_out ) const noexcept
	{
		p_out.advancedParametersEquilibrationNvt = _uiObjects._equilibrationNvt.layout;
		p_out.advancedParametersEquilibrationNpt = _uiObjects._equilibrationNpt.layout;
		p_out.advancedParametersProduction		 = _uiObjects._production.layout;
		p_out.advancedParametersMinimization	 = _uiObjects._layoutMinimization;
		p_out.advancedParametersSystemPrepartion = _uiObjects._layoutSystemPreparation;
	}
	void FormAdvanced::update( const EngineSpecificCommonInformation & p_data ) noexcept
	{
		_lastUpdateData = p_data;
		_fillEngineSpecificData();
	}
	QWidget * FormAdvanced::_uiSetupContainer( QWidget * p_container ) noexcept
	{
		{
			QVBoxLayout * qLayoutContainer = new QVBoxLayout( p_container );
			_thingsToDelete[ 0 ]		   = qLayoutContainer;
			qLayoutContainer->setContentsMargins( 0, 0, 0, 0 );
			_uiObjects.container = new QWidget;
			_thingsToDelete[ 1 ] = _uiObjects.container;
			qLayoutContainer->addWidget( _uiObjects.container );
		}

		_uiObjects.container->setContentsMargins( 0, 0, 0, 0 );
		QVBoxLayout * qLayoutContainer = new QVBoxLayout( _uiObjects.container );
		qLayoutContainer->setContentsMargins( 0, 0, 0, 0 );
		QScrollArea * qScroll	   = new QScrollArea();
		QWidget *	  qInnerScroll = new QWidget;
		qScroll->setWidget( qInnerScroll );
		qScroll->setFrameShape( QFrame::NoFrame );
		qScroll->setWidgetResizable( true );
		qLayoutContainer->addWidget( qScroll );

		return qInnerScroll;
	}
	void FormAdvanced::_uiAddSystem( QVBoxLayout * p_target ) noexcept
	{
		VTX::UI::QT::Util::addLabeledHLineSeparator( p_target, "System Preparation" );

		QHBoxLayout * systemReportButtonLayout = new QHBoxLayout;
		_uiObjects._systemReportLayout		   = new QVBoxLayout;
		p_target->addLayout( systemReportButtonLayout );
		systemReportButtonLayout->addLayout( _uiObjects._systemReportLayout );
		_uiObjects._buttonStartInputCheck = new QPushButton( "Check System" );
		systemReportButtonLayout->addWidget( _uiObjects._buttonStartInputCheck );

		_uiObjects._layoutSystemPreparation = new QFormLayout;
		p_target->addLayout( _uiObjects._layoutSystemPreparation );

		QWidget * label = nullptr;

		VTX::Tool::Mdprep::ui::get(
			E_COMMON_FIELDS_COMBOBOX ::bioForcefield, label, _uiObjects._fieldSystemBioForceField
		);
		_uiObjects._layoutSystemPreparation->addRow( label, _uiObjects._fieldSystemBioForceField );

		VTX::Tool::Mdprep::ui::get(
			E_COMMON_FIELDS_COMBOBOX ::chemForcefield, label, _uiObjects._fieldSystemChemForceField
		);
		_uiObjects._fieldSystemChemForceField->addItem( "Not supported by VTX yet" );
		_uiObjects._layoutSystemPreparation->addRow( label, _uiObjects._fieldSystemChemForceField );

		VTX::Tool::Mdprep::ui::get( E_COMMON_FIELDS_COMBOBOX ::waterModel, label, _uiObjects._fieldSystemWaterModel );
		_uiObjects._layoutSystemPreparation->addRow( label, _uiObjects._fieldSystemWaterModel );

		QFormLayout * boxLayout = new QFormLayout;
		p_target->addLayout( boxLayout );
		VTX::Tool::Mdprep::ui::get( E_COMMON_FIELDS_COMBOBOX ::boxShape, label, _uiObjects._fieldSystemBoxShape );
		boxLayout->addRow( label, _uiObjects._fieldSystemBoxShape );

		_fieldPlacer( 0 ) = _specificFieldGenerator( E_FIELD_SECTION::system );
		_fieldPlacer( 0 ).placeFields( boxLayout );

		p_target->addSpacerItem( new QSpacerItem( 0, 10 ) );
		_fieldsIons							= IonicFields( p_target, _parameters().system );
		_uiObjects._layoutSystemPreparation = new QFormLayout;
		p_target->addLayout( _uiObjects._layoutSystemPreparation );
	}
	void FormAdvanced::_uiAddExpertButton( QVBoxLayout * p_target ) noexcept
	{
		VTX::UI::QT::Util::addLabeledHLineSeparator( p_target, "" );

		QHBoxLayout * layout = new QHBoxLayout;
		p_target->addLayout( layout );

		VTX::UI::QT::Util::LabelWithHelper label(
			"Not finding what you need ?",
			"Pushing the button will open the simulation parameter file template so you can edit more advanced "
			"options. Do it only if you know what you are doing.",
			VTX::UI::QT::Util::LabelWithHelper ::E_QUESTIONMARK_POSITION::left
		);

		layout->addWidget( label );
		_uiObjects._buttonOpenExpertOptions = new QPushButton( "Expert Parameters" );
		layout->addWidget( _uiObjects._buttonOpenExpertOptions );
	}
	void FormAdvanced::_uiAddMinimization( QVBoxLayout * p_target ) noexcept
	{
		VTX::UI::QT::Util::addLabeledHLineSeparator( p_target, "Minimization" );
		_uiObjects._layoutMinimization = new QFormLayout;
		p_target->addLayout( _uiObjects._layoutMinimization );
		QWidget * label = nullptr;

		VTX::Tool::Mdprep::ui::get(
			E_COMMON_FIELDS_UINT64::stepNumberMinimization, label, _uiObjects._fieldMinimizationStepNumber
		);
		_uiObjects._layoutMinimization->addRow( label, _uiObjects._fieldMinimizationStepNumber );

		VTX::Tool::Mdprep::ui::get(
			E_COMMON_FIELDS_DOUBLE::minimizationStepSize, label, _uiObjects._fieldMinimizationStepSize
		);
		_uiObjects._layoutMinimization->addRow( label, _uiObjects._fieldMinimizationStepSize );

		VTX::Tool::Mdprep::ui::get(
			E_COMMON_FIELDS_DOUBLE::minimizationEnergyBreakpoint, label, _uiObjects._fieldMinimizationStopThreshold
		);
		_uiObjects._layoutMinimization->addRow( label, _uiObjects._fieldMinimizationStopThreshold );

		_fieldPlacer( 1 ) = _specificFieldGenerator( E_FIELD_SECTION::minimization );
		_fieldPlacer( 1 ).placeFields( _uiObjects._layoutMinimization );
	}
	void FormAdvanced::_uiAddStep( QVBoxLayout * p_target, const char * p_header, UiData::FieldSet & p_field ) noexcept
	{
		VTX::UI::QT::Util::addLabeledHLineSeparator( p_target, p_header );
		p_field.layout	= new QFormLayout;
		QWidget * label = nullptr;
		p_target->addLayout( p_field.layout );

		VTX::Tool::Mdprep::ui::get( E_COMMON_FIELDS_DOUBLE::dt, label, p_field.dt );
		p_field.layout->addRow( label, p_field.dt );
		VTX::Tool::Mdprep::ui::get( E_COMMON_FIELDS_UINT64::stepNumber, label, p_field.nsteps );
		p_field.layout->addRow( label, p_field.nsteps );

		VTX::Tool::Mdprep::ui::get( E_COMMON_FIELDS_UINT64::coordinateSaveInterval, label, p_field.saveInterval );
		p_field.layout->addRow( label, p_field.saveInterval );
	}
	MdEngineSpecificFieldPlacer & FormAdvanced::_fieldPlacer( const size_t & p_idx ) noexcept
	{
		return _eventManager.fieldPlacers.at( p_idx );
	}
	void FormAdvanced::_createAndPlaceUiItems( QWidget * p_container ) noexcept
	{
		QVBoxLayout * qLayoutMain = new QVBoxLayout( _uiSetupContainer( p_container ) );

		_uiAddSystem( qLayoutMain );
		_uiAddMinimization( qLayoutMain );
		_uiAddStep( qLayoutMain, "Equilibration Nvt", _uiObjects._equilibrationNvt );
		_fieldPlacer( 2 ) = _specificFieldGenerator( E_FIELD_SECTION::equilibrationNvt );
		_fieldPlacer( 2 ).placeFields( _uiObjects._equilibrationNvt.layout );

		_uiAddStep( qLayoutMain, "Equilibration Npt", _uiObjects._equilibrationNpt );
		_fieldPlacer( 3 ) = _specificFieldGenerator( E_FIELD_SECTION::equilibrationNpt );
		_fieldPlacer( 3 ).placeFields( _uiObjects._equilibrationNpt.layout );

		_uiAddStep( qLayoutMain, "Production run", _uiObjects._production );
		_fieldPlacer( 4 ) = _specificFieldGenerator( E_FIELD_SECTION::production );
		_fieldPlacer( 4 ).placeFields( _uiObjects._production.layout );

		_uiAddExpertButton( qLayoutMain );
	}
	namespace

	{
		template<typename T>
		void loadStepValue( T & p_ui, const MdParametersStepData & p_data ) noexcept
		{
			p_ui.nsteps->setText( QString::number( p_data.nsteps ) );
			p_ui.dt->setValue( p_data.dt );
			p_ui.saveInterval->setText( QString::number( p_data.saveInterval ) );
		}
	} // namespace
	void FormAdvanced::_loadValues( const MdParameters & p_defaults ) noexcept
	{
		_uiObjects._fieldMinimizationStepNumber->setText( QString::number( p_defaults.minimization.stepNumber ) );
		_uiObjects._fieldMinimizationStopThreshold->setValue( p_defaults.minimization.energyBreakpoint

		);
		_uiObjects._fieldMinimizationStepSize->setValue( p_defaults.minimization.stepSize );
		loadStepValue( _uiObjects._equilibrationNvt, p_defaults.nvt );
		loadStepValue( _uiObjects._equilibrationNpt, p_defaults.npt );
		loadStepValue( _uiObjects._production, p_defaults.prod );
	}
	namespace
	{
		void setMatchingValue( QComboBox * p_field, const std::string & p_value )
		{
			auto idx = p_field->findText( QString::fromStdString( p_value ) );
			if ( idx >= 0 )
				p_field->setCurrentIndex( idx );
		}
		void setMatchingValue( QComboBox * p_field, const int & p_value )
		{
			if ( p_value < p_field->count() )
				p_field->setCurrentIndex( p_value );
		}
	} // namespace
	void FormAdvanced::_fillEngineSpecificData() noexcept
	{
		_uiObjects._fieldSystemBoxShape->clear();
		for ( auto & it_shapeStr : _lastUpdateData.boxShapeCollection )
			_uiObjects._fieldSystemBoxShape->addItem( QString::fromStdString( it_shapeStr ) );
		setMatchingValue( _uiObjects._fieldSystemBoxShape, static_cast<int>( _parameters().system.boxShapeIdx ) );

		_uiObjects._fieldSystemBioForceField->clear();
		for ( auto & it_bioFfStr : _lastUpdateData.bioForcefieldCollection )
			_uiObjects._fieldSystemBioForceField->addItem( QString::fromStdString( it_bioFfStr ) );
		setMatchingValue( _uiObjects._fieldSystemBioForceField, _parameters().system.forcefieldBio );

		_uiObjects._fieldSystemWaterModel->clear();
		for ( auto & it_waterModelStr : _lastUpdateData.waterModels )
			_uiObjects._fieldSystemWaterModel->addItem( QString::fromStdString( it_waterModelStr ) );
		setMatchingValue( _uiObjects._fieldSystemWaterModel, _parameters().system.waterModel );

		_fieldPlacer( 0 ).update( EngineSpecificCommonInformationFieldUpdate {
			.field	  = E_ENGINE_SPECIFIC_COMMON_INFORMATION_FIELD::boxShape,
			.newValue = static_cast<size_t>( _uiObjects._fieldSystemBoxShape->currentIndex() ) } );

		_eventManager.connectBioForceField( _uiObjects._fieldSystemBioForceField, _uiObjects._systemReportLayout );
		_eventManager.connectBoxShape( _uiObjects._fieldSystemBoxShape );
		_eventManager.connectInputCheck( _uiObjects._buttonStartInputCheck );
	}

} // namespace VTX::Tool::Mdprep::ui::form_advanced
