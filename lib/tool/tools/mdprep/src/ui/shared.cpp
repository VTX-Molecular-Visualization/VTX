#include <qspinbox.h>
//
#include <ui/qt/util.hpp>
#include <ui/qt/validator.hpp>
//
#include "tools/mdprep/ui/shared.hpp"

namespace VTX::Tool::Mdprep::ui
{
	void get( const E_COMMON_FIELDS_COMBOBOX & p_enum, QWidget *& p_label, QComboBox *& p_field ) noexcept
	{
		switch ( p_enum )
		{
		case E_COMMON_FIELDS_COMBOBOX::bioForcefield:
			p_label = VTX::UI::QT::Util::LabelWithHelper(
				"Force Field for biological molecules",
				"Sets of values used by the engine to compute biological molecules motion.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			);
			break;
		case E_COMMON_FIELDS_COMBOBOX::chemForcefield:
			p_label = VTX::UI::QT::Util::LabelWithHelper(
				"Force Field for Small Organic molecules",
				"Sets of values used by the engine to compute small organic molecules motion.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			);
			break;
		case E_COMMON_FIELDS_COMBOBOX::boxShape:
			p_label = VTX::UI::QT::Util::LabelWithHelper(
				"System Periodic Box shape",
				"Defines the shape of the box used to set up periodic boundary conditions. The rule of thumbs it "
				"to choose a shape that is the closest match to the shape of the elements of interest.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			);
			break;
		case E_COMMON_FIELDS_COMBOBOX::waterModel:
			p_label = VTX::UI::QT::Util::LabelWithHelper(
				"Water model",
				"States how the water should be modelised. More complex water model allow more realistic water "
				"behavior, but with a huge computational cost.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			);
			break;
		default: break;
		}
		p_field = new QComboBox;
	}
	void get( const E_COMMON_FIELDS_DOUBLE & p_enum, QWidget *& p_label, QDoubleSpinBox *& field ) noexcept
	{
		switch ( p_enum )
		{
		case E_COMMON_FIELDS_DOUBLE::minimizationStepSize:
			p_label = VTX::UI::QT::Util::LabelWithHelper(
				"Minimization step size",
				"Defines how big the delta between each steps should be.<br>A bigger step size means quicker "
				"minimization but with reduced result stability.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			);
			field = new QDoubleSpinBox;
			field->setMinimum( 1e-4 );
			field->setMaximum( 1e1 );
			field->setSingleStep( 1e-3 );
			field->setDecimals( 4 );
			break;
		case E_COMMON_FIELDS_DOUBLE::minimizationEnergyBreakpoint:
			p_label = VTX::UI::QT::Util::LabelWithHelper(
				"Early termination threshold",
				"Energy delta between steps from which the minimization is considered <i>converged</i>.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			);
			field = new QDoubleSpinBox;
			field->setMinimum( 1e-1 );
			field->setMaximum( 1e6 );
			field->setSingleStep( 1e2 );
			field->setDecimals( 1 );
			break;
		case E_COMMON_FIELDS_DOUBLE::dt:

			p_label = VTX::UI::QT::Util::LabelWithHelper(
				"Time step interval (ns)",
				"Quantity of time passed between each steps. <br><br>2fs (0.002 ns) is a good default as it allows "
				"C-H bonds length to remain constant throughout the simulation.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			);
			field = new QDoubleSpinBox;
			field->setMinimum( 1e-3 );
			field->setMaximum( 1. );
			field->setSingleStep( 1e-3 );
			field->setDecimals( 3 );

			break;

		default: break;
		}
	}
	void get( const E_COMMON_FIELDS_UINT64 & p_enum, QWidget *& p_label, QLineEdit *& p_field ) noexcept
	{
		switch ( p_enum )
		{
		case E_COMMON_FIELDS_UINT64::stepNumberMinimization:

			p_label = VTX::UI::QT::Util::LabelWithHelper(
				"Number of Minimization Steps",
				"Dictates how long the minimization can be. Bigger systems need higher number of "
				"steps.<br>Minimization shall still end earlier if the energy plateau is reached.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			);
			break;
		case E_COMMON_FIELDS_UINT64::stepNumber:

			p_label = VTX::UI::QT::Util::LabelWithHelper(
				"Number of computed steps",
				"Defines how long this part of the simulation will be.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			);
			break;
		case E_COMMON_FIELDS_UINT64::coordinateSaveInterval:
			p_label = VTX::UI::QT::Util::LabelWithHelper(
				"Coordinate Save Interval",
				"Number of steps between each snapshot of system coordinate. The smaller the number, the smoother the "
				"output will look but with larger file. A value of 0 will instruct the engine not to save is at all.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			);

		default: break;
		}
		p_field = new QLineEdit;
		p_field->setValidator( new VTX::UI::QT::QUInt64Validator( p_field ) );
	}
} // namespace VTX::Tool::Mdprep::ui
