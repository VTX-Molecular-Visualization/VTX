#include <qdialog.h>
//

#include "tools/mdprep/gateway/form_data.hpp"
//
#include "tools/mdprep/ui/basic_form_settings_dialog.hpp"

namespace VTX::Tool::Mdprep::ui
{
	using namespace VTX::Tool::Mdprep::Gateway;

	BasicFormSettingsDialog::BasicFormSettingsDialog(
		QWidget *			   p_parent,
		MdParametersStepData & p_dataFields,
		uint64_t &			   p_nstxout
	) :
		_dataFields( &p_dataFields ),
		_nstxout( &p_nstxout )
	{
		_setupUi( p_parent );
	}
	BasicFormSettingsDialog::BasicFormSettingsDialog( QWidget * p_parent, MdParametersStepData & p_dataFields ) {}
	void BasicFormSettingsDialog::_setupUi( QWidget * p_parent )
	{
		_uiObjects.container = new QDialog( p_parent );
		_uiObjects.container->show();
	}
} // namespace VTX::Tool::Mdprep::ui
