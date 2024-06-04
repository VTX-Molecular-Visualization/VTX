#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_BASIC_FORM_SETTINGS_DIALOG__
#define __VTX_TOOL_TOOLS_MDPREP_UI_BASIC_FORM_SETTINGS_DIALOG__

#include <optional>

class QWidget;

namespace VTX::Tool::Mdprep::Gateway
{
	struct MdParametersStepData;
}

namespace VTX::Tool::Mdprep::ui
{
	using namespace VTX::Tool::Mdprep::Gateway;

	// Class responsible for managing a settings panel for advanced options of an MD step, but from the basic form.
	class BasicFormSettingsDialog
	{
	  public:
		BasicFormSettingsDialog() = default;
		BasicFormSettingsDialog( QWidget * p_parent, MdParametersStepData & p_dataFields, uint64_t & p_nstxout );
		BasicFormSettingsDialog( QWidget * p_parent, MdParametersStepData & p_dataFields );

		BasicFormSettingsDialog( BasicFormSettingsDialog && )				   = delete;
		BasicFormSettingsDialog( const BasicFormSettingsDialog & )			   = delete;
		BasicFormSettingsDialog & operator=( BasicFormSettingsDialog && )	   = delete;
		BasicFormSettingsDialog & operator=( const BasicFormSettingsDialog & ) = delete;
		~BasicFormSettingsDialog() noexcept;

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
