#include <qformlayout.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qscrollarea.h>
#include <qspinbox.h>
#include <ui/qt/util.hpp>
#include <util/logger.hpp>
//
#include "tools/mdprep/gateway/form_data.hpp"
#include "tools/mdprep/ui/shared.hpp"
//
#include "tools/mdprep/ui/form_advanced.hpp"

namespace VTX::Tool::Mdprep::ui
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
	FormAdvanced::FormAdvanced( QWidget * p_container, const MdParameters & p_defaults ) : _parameters( p_defaults )
	{
		_createAndPlaceUiItems( p_container );
		_loadValues( p_defaults );
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
		_uiObjects._layoutSystemPreparation = new QFormLayout;
		p_target->addLayout( _uiObjects._layoutSystemPreparation );

		QWidget * label = nullptr;
		VTX::Tool::Mdprep::ui::get( E_COMMON_FIELDS_COMBOBOX ::boxShape, label, _uiObjects._fieldSystemBoxShape );
		_uiObjects._layoutSystemPreparation->addRow( label, _uiObjects._fieldSystemBoxShape );

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
	//_uiObjects._equilibrationNvt.nstvout = addUInt64Field(
	//	_uiObjects._equilibrationNvt.layout,
	//	"Velocities Save Interval",
	//	"Number of steps between each snapshot of system velocities. Might be usefull for more precise analysis, "
	//	"but rarely on equilibration steps. A value of 0 will instruct the engine not to save is at all."
	//);
	//_uiObjects._equilibrationNvt.nstenergy = addUInt64Field(
	//	_uiObjects._equilibrationNvt.layout,
	//	"Energy Save Interval",
	//	"Number of steps between each snapshot of system energy. Might be usefull for more precise analysis, "
	//	"but rarely on equilibration steps. A value of 0 will instruct the engine not to save is at all."
	//);
	//_uiObjects._equilibrationNvt.nstlog = addUInt64Field(
	//	_uiObjects._equilibrationNvt.layout,
	//	"Log Write Interval",
	//	"Number of steps between each Log entry. Might be usefull to follow how MD simulation unfold. A value of 0 "
	//	"will instruct the engine not to save is at all."
	//);
	void FormAdvanced::_createAndPlaceUiItems( QWidget * p_container ) noexcept
	{
		QVBoxLayout * qLayoutMain = new QVBoxLayout( _uiSetupContainer( p_container ) );
		_uiAddSystem( qLayoutMain );
		_uiAddMinimization( qLayoutMain );
		_uiAddStep( qLayoutMain, "Equilibration Nvt", _uiObjects._equilibrationNvt );
		_uiAddStep( qLayoutMain, "Equilibration Npt", _uiObjects._equilibrationNpt );
		_uiAddStep( qLayoutMain, "Production run", _uiObjects._production );
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

		return;
		//_uiObjects._fieldMinimizationStepSize->setText( QString::number(
		// p_defaults.minimization.stepNumberMinimization ) ); _uiObjects._fieldMinimizationStopThreshold->setText(
		// QString::number( p_defaults.minimization.energyBreakpoint
		//)
		//);
		//_uiObjects._fieldProductionNstxoutCompressed->setText( QString::number( p_defaults.prodNstxout_compressed ) );

		_uiObjects._equilibrationNvt.dt->setValue( p_defaults.nvt.dt );
		_uiObjects._equilibrationNvt.nsteps->setText( QString::number( p_defaults.nvt.nsteps ) );
		//_uiObjects._equilibrationNvt.nstxout->setText( QString::number( p_defaults.nvt.nstxout ) );
		//_uiObjects._equilibrationNvt.nstvout->setText( QString::number( p_defaults.nvt.nstvout ) );
		//_uiObjects._equilibrationNvt.nstlog->setText( QString::number( p_defaults.nvt.nstlog ) );
		//_uiObjects._equilibrationNvt.nstenergy->setText( QString::number( p_defaults.nvt.nstenergy ) );

		_uiObjects._equilibrationNpt.dt->setValue( p_defaults.npt.dt );
		_uiObjects._equilibrationNpt.nsteps->setText( QString::number( p_defaults.npt.nsteps ) );
		//_uiObjects._equilibrationNpt.nstxout->setText( QString::number( p_defaults.npt.nstxout ) );
		//_uiObjects._equilibrationNpt.nstvout->setText( QString::number( p_defaults.npt.nstvout ) );
		//_uiObjects._equilibrationNpt.nstlog->setText( QString::number( p_defaults.npt.nstlog ) );
		//_uiObjects._equilibrationNpt.nstenergy->setText( QString::number( p_defaults.npt.nstenergy ) );

		_uiObjects._production.dt->setValue( p_defaults.prod.dt );
		_uiObjects._production.nsteps->setText( QString::number( p_defaults.prod.nsteps ) );
		//_uiObjects._production.nstxout->setText( QString::number( p_defaults.prod.nstxout ) );
		//_uiObjects._production.nstvout->setText( QString::number( p_defaults.prod.nstvout ) );
		//_uiObjects._production.nstlog->setText( QString::number( p_defaults.prod.nstlog ) );
		//_uiObjects._production.nstenergy->setText( QString::number( p_defaults.prod.nstenergy ) );
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
		setMatchingValue( _uiObjects._fieldSystemBoxShape, static_cast<int>( _parameters.system.boxShapeIdx ) );

		_uiObjects._fieldSystemBioForceField->clear();
		for ( auto & it_bioFfStr : _lastUpdateData.bioForcefieldCollection )
			_uiObjects._fieldSystemBioForceField->addItem( QString::fromStdString( it_bioFfStr ) );
		setMatchingValue( _uiObjects._fieldSystemBioForceField, _parameters.system.forcefieldBio );

		_uiObjects._fieldSystemWaterModel->clear();
		for ( auto & it_waterModelStr : _lastUpdateData.waterModels )
			_uiObjects._fieldSystemWaterModel->addItem( QString::fromStdString( it_waterModelStr ) );
		setMatchingValue( _uiObjects._fieldSystemWaterModel, _parameters.system.waterModel );
	}
	void FormAdvanced::_sendUpdateSignal() noexcept
	{
		_eventToggler.toggle( _uiObjects );

		MdAdvancedDataSample dataSample;
		dataSample.dtNvt		  = _uiObjects._equilibrationNvt.dt->text().toDouble();
		dataSample.stepNumberNvt  = static_cast<uint64_t>( _uiObjects._equilibrationNvt.nsteps->text().toDouble() );
		dataSample.dtNpt		  = _uiObjects._equilibrationNpt.dt->text().toDouble();
		dataSample.stepNumberNpt  = static_cast<uint64_t>( _uiObjects._equilibrationNpt.nsteps->text().toDouble() );
		dataSample.dtProd		  = _uiObjects._production.dt->text().toDouble();
		dataSample.stepNumberProd = static_cast<uint64_t>( _uiObjects._production.nsteps->text().toDouble() );
		_updateCallback( dataSample );

		_eventToggler.toggle( _uiObjects );
	}
	FormAdvanced::EventToggler::EventToggler( std::function<void()> p_ ) : _lambdaToConnect( std::move( p_ ) ) {}
	void FormAdvanced::EventToggler::toggle( UiData & p_ui ) noexcept
	{
		if ( _eventsOn )
			_disconnect( p_ui );
		else
			_connect( p_ui );
		_eventsOn ^= true;
	}
	void FormAdvanced::EventToggler::_connect( UiData & p_uiObjects ) noexcept
	{
		QLineEdit::connect( p_uiObjects._equilibrationNvt.nsteps, &QLineEdit::textChanged, _lambdaToConnect );
		QDoubleSpinBox::connect( p_uiObjects._equilibrationNvt.dt, &QDoubleSpinBox::valueChanged, _lambdaToConnect );
		QLineEdit::connect( p_uiObjects._equilibrationNpt.nsteps, &QLineEdit::textChanged, _lambdaToConnect );
		QDoubleSpinBox::connect( p_uiObjects._equilibrationNpt.dt, &QDoubleSpinBox::valueChanged, _lambdaToConnect );
		QLineEdit::connect( p_uiObjects._production.nsteps, &QLineEdit::textChanged, _lambdaToConnect );
		QDoubleSpinBox::connect( p_uiObjects._production.dt, &QDoubleSpinBox::valueChanged, _lambdaToConnect );
	}
	void FormAdvanced::EventToggler::_disconnect( UiData & p_uiObjects ) noexcept
	{
		p_uiObjects._equilibrationNvt.nsteps->disconnect();
		p_uiObjects._equilibrationNvt.dt->disconnect();
		p_uiObjects._equilibrationNpt.nsteps->disconnect();
		p_uiObjects._equilibrationNpt.dt->disconnect();
		p_uiObjects._production.nsteps->disconnect();
		p_uiObjects._production.dt->disconnect();
	}
} // namespace VTX::Tool::Mdprep::ui
