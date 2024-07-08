#include <ui/qt/util.hpp>
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
} // namespace VTX::Tool::Mdprep::ui
