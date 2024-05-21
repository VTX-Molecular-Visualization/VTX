#include <qformlayout.h>
#include <qlineedit.h>
//
#include "tools/mdprep/ui/basic_form.hpp"

namespace VTX::Tool::Mdprep::ui
{
	void MdBasicParamForm::setupUi( QWidget * p_container )
	{
		QFormLayout * qform			 = new QFormLayout( p_container );
		_fieldMinimizationStepNumber = new QLineEdit();

		qform->addRow( "Min step num", _fieldMinimizationStepNumber );
	}
	void MdBasicParamForm::get( MdParametersBasic & ) const noexcept {}
} // namespace VTX::Tool::Mdprep::ui
