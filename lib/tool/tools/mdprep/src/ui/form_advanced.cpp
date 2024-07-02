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
	void FormAdvanced::get( MdParameters & p_out ) const noexcept { p_out = _parameters; }
	void FormAdvanced::get( FormLayouts & p_out ) const noexcept
	{
		p_out.advancedParametersEquilibrationNvt = _uiObjects._equilibrationNvt.layout;
		p_out.advancedParametersEquilibrationNpt = _uiObjects._equilibrationNpt.layout;
		p_out.advancedParametersProduction		 = _uiObjects._production.layout;
		p_out.advancedParametersMinimization	 = _uiObjects._layoutMinimization;
		p_out.advancedParametersSystemPrepartion = _uiObjects._layoutSystemPreparation;
	}
	void FormAdvanced::update( const EngineSpecificCommonInformation & p_data ) noexcept { _lastUpdateData = p_data; }
	void FormAdvanced::_createAndPlaceUiItems( QWidget * p_container ) noexcept
	{
		p_container->setContentsMargins( 0, 0, 0, 0 );
		QVBoxLayout * qLayoutContainer = new QVBoxLayout( p_container );
		_thingsToDelete[ 0 ]		   = qLayoutContainer;
		qLayoutContainer->setContentsMargins( 0, 0, 0, 0 );
		_uiObjects.container = new QWidget;
		_thingsToDelete[ 1 ] = _uiObjects.container;
		qLayoutContainer	 = new QVBoxLayout( _uiObjects.container );
		qLayoutContainer->setContentsMargins( 0, 0, 0, 0 );
		QScrollArea * qScroll	   = new QScrollArea();
		QWidget *	  qInnerScroll = new QWidget;
		qScroll->setWidget( qInnerScroll );
		qScroll->setFrameShape( QFrame::NoFrame );
		qScroll->setWidgetResizable( true );
		qLayoutContainer->addWidget( qScroll );
		QVBoxLayout * qLayoutMain = new QVBoxLayout( qInnerScroll );

		VTX::UI::QT::Util::addLabeledHLineSeparator( qLayoutMain, "System Preparation" );
		_uiObjects._layoutSystemPreparation = new QFormLayout;
		qLayoutMain->addLayout( _uiObjects._layoutSystemPreparation );

		VTX::UI::QT::Util::addLabeledHLineSeparator( qLayoutMain, "Minimization" );
		_uiObjects._layoutMinimization = new QFormLayout;
		qLayoutMain->addLayout( _uiObjects._layoutMinimization );
		_uiObjects._fieldMinimizationStepSize = addUInt64Field(
			_uiObjects._layoutMinimization,
			"Step size",
			"Rule the speed of the minimization. Larger step mean faster convergeance but grainer results."
		);
		_uiObjects._fieldMinimizationStopThreshold = addUInt64Field(
			_uiObjects._layoutMinimization,
			"Convergeance Energy (kJ/mol/nm)",
			"Energy delta from between steps from which the system is considered converged. Therefore allow for early "
			"termination of the minimization phase.\nHigher energy score mean quicker termination but less stable "
			"system."
		);

		VTX::UI::QT::Util::addLabeledHLineSeparator( qLayoutMain, "Equilibration Nvt" );
		_uiObjects._equilibrationNvt.layout = new QFormLayout;
		qLayoutMain->addLayout( _uiObjects._equilibrationNvt.layout );
		// addUInt64FieldWithRightLabel(
		//	_uiObjects._equilibrationNvt.layout,
		//	"Number of Nvt Equilibration Steps",
		//	"Number of Equilibration steps with constant volume and temperature.\nThis is the first equilibration "
		//	"step. Bigger systems need higher number of steps.\n",
		//	&_uiObjects._equilibrationNvt.nsteps,
		//	&_uiObjects._equilibrationNvt.labelNsteps
		//);
		_uiObjects._equilibrationNvt.dt = addDoubleTextbox(
			_uiObjects._equilibrationNvt.layout,
			"Step Time Interval (ps)",
			"Interval between each steps in picoseconds. Usually 0.002 is the goto solution as it allows to freeze C-H "
			"bond elongation and speed up calculations.",
			0.002,
			0.001,
			0.,
			0.1,
			3
		);
		_uiObjects._equilibrationNvt.nstxout = addUInt64Field(
			_uiObjects._equilibrationNvt.layout,
			"Coordinate Save Interval",
			"Number of steps between each snapshot of system coordinate. The smaller the number, the smoother the "
			"output will look but with larger file. A value of 0 will instruct the engine not to save is at all."
		);
		_uiObjects._equilibrationNvt.nstvout = addUInt64Field(
			_uiObjects._equilibrationNvt.layout,
			"Velocities Save Interval",
			"Number of steps between each snapshot of system velocities. Might be usefull for more precise analysis, "
			"but rarely on equilibration steps. A value of 0 will instruct the engine not to save is at all."
		);
		_uiObjects._equilibrationNvt.nstenergy = addUInt64Field(
			_uiObjects._equilibrationNvt.layout,
			"Energy Save Interval",
			"Number of steps between each snapshot of system energy. Might be usefull for more precise analysis, "
			"but rarely on equilibration steps. A value of 0 will instruct the engine not to save is at all."
		);
		_uiObjects._equilibrationNvt.nstlog = addUInt64Field(
			_uiObjects._equilibrationNvt.layout,
			"Log Write Interval",
			"Number of steps between each Log entry. Might be usefull to follow how MD simulation unfold. A value of 0 "
			"will instruct the engine not to save is at all."
		);

		VTX::UI::QT::Util::addLabeledHLineSeparator( qLayoutMain, "Equilibration Npt" );
		_uiObjects._equilibrationNpt.layout = new QFormLayout;
		qLayoutMain->addLayout( _uiObjects._equilibrationNpt.layout );
		// addUInt64FieldWithRightLabel(
		//	_uiObjects._equilibrationNpt.layout,
		//	"Number of Npt Equilibration Steps",
		//	"Number of Equilibration steps with constant volume and temperature.\nThis is the first equilibration "
		//	"step. Bigger systems need higher number of steps. A value of 0 will instruct the engine not to save is at "
		//	"all.",
		//	&_uiObjects._equilibrationNpt.nsteps,
		//	&_uiObjects._equilibrationNpt.labelNsteps
		//);
		_uiObjects._equilibrationNpt.dt = addDoubleTextbox(
			_uiObjects._equilibrationNpt.layout,
			"Step Time Interval (ps)",
			"Interval between each steps in picoseconds. Usually 0.002 is the goto solution as it allows to freeze C-H "
			"bond elongation and speed up calculations.",
			0.002,
			0.001,
			0.,
			0.1,
			3
		);
		_uiObjects._equilibrationNpt.nstxout = addUInt64Field(
			_uiObjects._equilibrationNpt.layout,
			"Coordinate Save Interval",
			"Number of steps between each snapshot of system coordinate. The smaller the number, the smoother the "
			"output will look but with larger file. A value of 0 will instruct the engine not to save is at all."
		);
		_uiObjects._equilibrationNpt.nstvout = addUInt64Field(
			_uiObjects._equilibrationNpt.layout,
			"Velocities Save Interval",
			"Number of steps between each snapshot of system velocities. Might be usefull for more precise analysis, "
			"but rarely on equilibration steps. A value of 0 will instruct the engine not to save is at all."
		);
		_uiObjects._equilibrationNpt.nstenergy = addUInt64Field(
			_uiObjects._equilibrationNpt.layout,
			"Energy Save Interval",
			"Number of steps between each snapshot of system energy. Might be usefull for more precise analysis, "
			"but rarely on equilibration steps. A value of 0 will instruct the engine not to save is at all."
		);
		_uiObjects._equilibrationNpt.nstlog = addUInt64Field(
			_uiObjects._equilibrationNpt.layout,
			"Log Write Interval",
			"Number of steps between each Log entry. Might be usefull to follow how MD simulation unfold. A value of 0 "
			"will instruct the engine not to save is at all."
		);

		VTX::UI::QT::Util::addLabeledHLineSeparator( qLayoutMain, "Production run" );
		_uiObjects._production.layout = new QFormLayout;
		qLayoutMain->addLayout( _uiObjects._production.layout );
		// addUInt64FieldWithRightLabel(
		//	qLayoutProductionForm,
		//	"Number of Production Steps",
		//	"Number of Production steps to perform.",
		//	&_uiObjects._production.nsteps,
		//	&_uiObjects._production.labelNsteps
		//);
		_uiObjects._production.dt = addDoubleTextbox(
			_uiObjects._production.layout,
			"Step Time Interval (ps)",
			"Interval between each steps in picoseconds. Usually 0.002 is the goto solution as it allows to freeze C-H "
			"bond elongation and speed up calculations.",
			0.002,
			0.001,
			0.,
			0.1,
			3
		);

		_uiObjects._production.nstxout = addUInt64Field(
			_uiObjects._production.layout,
			"Coordinate Save Interval",
			"Number of steps between each snapshot of system coordinate. The smaller the number, the smoother the "
			"output will look but with larger file. A value of 0 will instruct the engine not to save is at all."
		);
		_uiObjects._production.nstvout = addUInt64Field(
			_uiObjects._production.layout,
			"Velocities Save Interval",
			"Number of steps between each snapshot of system velocities. Might be usefull for more precise analysis. A "
			"value of 0 will instruct the engine not to save is at all."
		);
		_uiObjects._production.nstenergy = addUInt64Field(
			_uiObjects._production.layout,
			"Energy Save Interval",
			"Number of steps between each snapshot of system energy. Might be usefull for more precise analysis. A "
			"value of 0 will instruct the engine not to save is at all."
		);
		_uiObjects._production.nstlog = addUInt64Field(
			_uiObjects._production.layout,
			"Log Write Interval",
			"Number of steps between each Log entry. Might be usefull to follow how MD simulation unfold. A value of 0 "
			"will instruct the engine not to save is at all."
		);
		_uiObjects._fieldProductionNstxoutCompressed = addUInt64Field(
			_uiObjects._production.layout,
			"Compressed Coordinate Save Interval",
			"Number of steps between each snapshot of coordinates in compressed format. Preferred way of saving as it "
			"is lighter and can be red by VTX anyway. A value of 0 will instruct the engine not to save is at all."
		);
	}
	void FormAdvanced::_loadValues( const MdParameters & p_defaults ) noexcept
	{
		return;
		_uiObjects._fieldMinimizationStepSize->setText( QString::number( p_defaults.minimization.stepNumber ) );
		_uiObjects._fieldMinimizationStopThreshold->setText( QString::number( p_defaults.minimization.energyBreakpoint )
		);
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
