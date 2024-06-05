#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_BASIC_FORM_SETTINGS_DIALOG__
#define __VTX_TOOL_TOOLS_MDPREP_UI_BASIC_FORM_SETTINGS_DIALOG__

#include <optional>

class QWidget;
class QLineEdit;
class QDoubleSpinBox;
class QPushButton;

namespace VTX::Tool::Mdprep::Gateway
{
	struct MdParametersStepData;
}

namespace VTX::Tool::Mdprep::ui
{
	using namespace VTX::Tool::Mdprep::Gateway;

	class MdEngineSpecificFieldPlacer;
	// Class responsible for managing a settings panel for advanced options of an MD step, but from the basic form.
	class BasicFormSettingsDialog
	{
	  public:
		BasicFormSettingsDialog() = default;
		BasicFormSettingsDialog(
			QWidget *					p_parent,
			const char *				p_title,
			MdParametersStepData &		p_dataFields,
			MdEngineSpecificFieldPlacer p_engineFieldPlacer,
			uint64_t &					p_nstxout
		);
		BasicFormSettingsDialog(
			QWidget *					p_parent,
			const char *				p_title,
			MdParametersStepData &		p_dataFields,
			MdEngineSpecificFieldPlacer p_engineFieldPlacer
		);

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
			QDialog *		 container		   = nullptr;
			QDoubleSpinBox * fieldDt		   = nullptr;
			QLineEdit *		 fieldSaveInterval = nullptr;
			QPushButton *	 buttonApply	   = nullptr;
			QPushButton *	 buttonCancel	   = nullptr;
		};
		UiObjects					_uiObjects;
		MdEngineSpecificFieldPlacer _engineFieldPlacer;

		void _setupUi( QWidget * p_parent, const char * p_title );
		void _setupSlots() noexcept;
		void _applyChanges() noexcept;
		void _cancelChanges() noexcept;
	};
} // namespace VTX::Tool::Mdprep::ui

#endif
