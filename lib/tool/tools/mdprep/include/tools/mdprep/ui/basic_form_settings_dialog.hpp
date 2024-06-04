#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_BASIC_FORM__
#define __VTX_TOOL_TOOLS_MDPREP_UI_BASIC_FORM__

class QWidget;

namespace VTX::Tool::Mdprep::Gateway
{
	struct MdParametersStepData;
}

namespace VTX::Tool::Mdprep::ui
{
	using namespace VTX::Tool::Mdprep::Gateway;

	class BasicFormSettingsDialog
	{
	  public:
		BasicFormSettingsDialog( QWidget * p_parent, MdParametersStepData & p_dataFields, uint64_t & p_nstxout );
		BasicFormSettingsDialog( QWidget * p_parent, MdParametersStepData & p_dataFields );

	  private:
		MdParametersStepData *	  _dataFields = nullptr;
		std::optional<uint64_t *> _nstxout;

		struct UiObjects
		{
			QDialog * container = nullptr;
		};
		UiObjects _uiObjects;

		void _setupUi( QWidget * p_parent );
	};
} // namespace VTX::Tool::Mdprep::ui

#endif
