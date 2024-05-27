#include <qformlayout.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qscrollarea.h>
#include <qspinbox.h>
#include <ui/qt/util.hpp>
//
#include "tools/mdprep/ui/form_data.hpp"
//
#include "tools/mdprep/ui/advanced_form.hpp"

namespace VTX::Tool::Mdprep::ui
{
	using namespace VTX::UI::QT::Util;
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
			p_dest->addRow( VTX::UI::QT::Util::createLabelWithHelpTooltip( p_label, p_tooltip ), out );
			return out;
		}
	} // namespace
	void MdAdvancedParamForm::setupUi( QWidget * p_container, const MdParametersAdvanced & p_defaults )
	{
		_createAndPlaceUiItems( p_container );
		_loadDefaultValues( p_defaults );
	}
	void MdAdvancedParamForm::subscribe( UpdateCallback ) noexcept {}
	void MdAdvancedParamForm::get( MdParametersAdvanced & ) const noexcept {}
	void MdAdvancedParamForm::_createAndPlaceUiItems( QWidget * p_container ) noexcept
	{
		p_container->setContentsMargins( 0, 0, 0, 0 );
		QVBoxLayout * qLayoutContainer = new QVBoxLayout( p_container );
		qLayoutContainer->setContentsMargins( 0, 0, 0, 0 );
		QScrollArea * qScroll	   = new QScrollArea();
		QWidget *	  qInnerScroll = new QWidget;
		qScroll->setWidget( qInnerScroll );
		qScroll->setFrameShape( QFrame::NoFrame );
		qScroll->setWidgetResizable( true );
		qLayoutContainer->addWidget( qScroll );
		QVBoxLayout * qLayoutMain = new QVBoxLayout( qInnerScroll );

		VTX::UI::QT::Util::addLabeledHLineSeparator( qLayoutMain, "Minimization" );
		QFormLayout * qLayoutMinimizationForm = new QFormLayout;
		qLayoutMain->addLayout( qLayoutMinimizationForm );
		_fieldMinimizationStepSize = addUInt64Field(
			qLayoutMinimizationForm,
			"Step size",
			"Rule the speed of the minimization. Larger step mean faster convergeance but grainer results."
		);
		_fieldMinimizationStopThreshold = addUInt64Field(
			qLayoutMinimizationForm,
			"Convergeance Energy (kJ/mol/nm)",
			"Energy delta from between steps from which the system is considered converged. Therefore allow for early "
			"termination of the minimization phase.\nHigher energy score mean quicker termination but less stable "
			"system."
		);

		VTX::UI::QT::Util::addLabeledHLineSeparator( qLayoutMain, "Equilibration Nvt" );
		QFormLayout * qLayoutEquilibrationNvtForm = new QFormLayout;
		qLayoutMain->addLayout( qLayoutEquilibrationNvtForm );
		addUInt64FieldWithRightLabel(
			qLayoutEquilibrationNvtForm,
			"Number of Nvt Equilibration Steps",
			"Number of Equilibration steps with constant volume and temperature.\nThis is the first equilibration "
			"step. Bigger systems need higher number of steps.\n",
			&_equilibrationNvt.nsteps,
			&_equilibrationNvt.labelNsteps
		);
		_equilibrationNvt.dt = addDoubleTextbox(
			qLayoutEquilibrationNvtForm,
			"Step Time Interval (ps)",
			"Interval between each steps in picoseconds. Usually 0.002 is the goto solution as it allows to freeze C-H "
			"bond elongation and speed up calculations.",
			0.002,
			0.001,
			0.,
			0.1,
			3
		);
		_equilibrationNvt.nstxout = addUInt64Field(
			qLayoutEquilibrationNvtForm,
			"Coordinate Save Interval",
			"Number of steps between each snapshot of system coordinate. The smaller the number, the smoother the "
			"output will look but with larger file. A value of 0 will instruct the engine not to save is at all."
		);
		_equilibrationNvt.nstvout = addUInt64Field(
			qLayoutEquilibrationNvtForm,
			"Velocities Save Interval",
			"Number of steps between each snapshot of system velocities. Might be usefull for more precise analysis, "
			"but rarely on equilibration steps. A value of 0 will instruct the engine not to save is at all."
		);
		_equilibrationNvt.nstenergy = addUInt64Field(
			qLayoutEquilibrationNvtForm,
			"Energy Save Interval",
			"Number of steps between each snapshot of system energy. Might be usefull for more precise analysis, "
			"but rarely on equilibration steps. A value of 0 will instruct the engine not to save is at all."
		);
		_equilibrationNvt.nstlog = addUInt64Field(
			qLayoutEquilibrationNvtForm,
			"Log Write Interval",
			"Number of steps between each Log entry. Might be usefull to follow how MD simulation unfold. A value of 0 "
			"will instruct the engine not to save is at all."
		);

		VTX::UI::QT::Util::addLabeledHLineSeparator( qLayoutMain, "Equilibration Npt" );
		QFormLayout * qLayoutEquilibrationNptForm = new QFormLayout;
		qLayoutMain->addLayout( qLayoutEquilibrationNptForm );
		addUInt64FieldWithRightLabel(
			qLayoutEquilibrationNptForm,
			"Number of Npt Equilibration Steps",
			"Number of Equilibration steps with constant volume and temperature.\nThis is the first equilibration "
			"step. Bigger systems need higher number of steps. A value of 0 will instruct the engine not to save is at "
			"all.",
			&_equilibrationNpt.nsteps,
			&_equilibrationNpt.labelNsteps
		);
		_equilibrationNpt.dt = addDoubleTextbox(
			qLayoutEquilibrationNptForm,
			"Step Time Interval (ps)",
			"Interval between each steps in picoseconds. Usually 0.002 is the goto solution as it allows to freeze C-H "
			"bond elongation and speed up calculations.",
			0.002,
			0.001,
			0.,
			0.1,
			3
		);
		_equilibrationNpt.nstxout = addUInt64Field(
			qLayoutEquilibrationNptForm,
			"Coordinate Save Interval",
			"Number of steps between each snapshot of system coordinate. The smaller the number, the smoother the "
			"output will look but with larger file. A value of 0 will instruct the engine not to save is at all."
		);
		_equilibrationNpt.nstvout = addUInt64Field(
			qLayoutEquilibrationNptForm,
			"Velocities Save Interval",
			"Number of steps between each snapshot of system velocities. Might be usefull for more precise analysis, "
			"but rarely on equilibration steps. A value of 0 will instruct the engine not to save is at all."
		);
		_equilibrationNpt.nstenergy = addUInt64Field(
			qLayoutEquilibrationNptForm,
			"Energy Save Interval",
			"Number of steps between each snapshot of system energy. Might be usefull for more precise analysis, "
			"but rarely on equilibration steps. A value of 0 will instruct the engine not to save is at all."
		);
		_equilibrationNpt.nstlog = addUInt64Field(
			qLayoutEquilibrationNptForm,
			"Log Write Interval",
			"Number of steps between each Log entry. Might be usefull to follow how MD simulation unfold. A value of 0 "
			"will instruct the engine not to save is at all."
		);

		VTX::UI::QT::Util::addLabeledHLineSeparator( qLayoutMain, "Production run" );
		QFormLayout * qLayoutProductionForm = new QFormLayout;
		qLayoutMain->addLayout( qLayoutProductionForm );
		addUInt64FieldWithRightLabel(
			qLayoutProductionForm,
			"Number of Production Steps",
			"Number of Production steps to perform.",
			&_production.nsteps,
			&_production.labelNsteps
		);
		_production.dt = addDoubleTextbox(
			qLayoutProductionForm,
			"Step Time Interval (ps)",
			"Interval between each steps in picoseconds. Usually 0.002 is the goto solution as it allows to freeze C-H "
			"bond elongation and speed up calculations.",
			0.002,
			0.001,
			0.,
			0.1,
			3
		);
		_production.nstxout = addUInt64Field(
			qLayoutProductionForm,
			"Coordinate Save Interval",
			"Number of steps between each snapshot of system coordinate. The smaller the number, the smoother the "
			"output will look but with larger file. A value of 0 will instruct the engine not to save is at all."
		);
		_production.nstvout = addUInt64Field(
			qLayoutProductionForm,
			"Velocities Save Interval",
			"Number of steps between each snapshot of system velocities. Might be usefull for more precise analysis. A "
			"value of 0 will instruct the engine not to save is at all."
		);
		_production.nstenergy = addUInt64Field(
			qLayoutProductionForm,
			"Energy Save Interval",
			"Number of steps between each snapshot of system energy. Might be usefull for more precise analysis. A "
			"value of 0 will instruct the engine not to save is at all."
		);
		_production.nstlog = addUInt64Field(
			qLayoutProductionForm,
			"Log Write Interval",
			"Number of steps between each Log entry. Might be usefull to follow how MD simulation unfold. A value of 0 "
			"will instruct the engine not to save is at all."
		);
		_fieldProductionNstxoutCompressed = addUInt64Field(
			qLayoutProductionForm,
			"Compressed Coordinate Save Interval",
			"Number of steps between each snapshot of coordinates in compressed format. Preferred way of saving as it "
			"is lighter and can be red by VTX anyway. A value of 0 will instruct the engine not to save is at all."
		);
	}
	void MdAdvancedParamForm::_loadDefaultValues( const MdParametersAdvanced & p_defaults ) noexcept {}
} // namespace VTX::Tool::Mdprep::ui
