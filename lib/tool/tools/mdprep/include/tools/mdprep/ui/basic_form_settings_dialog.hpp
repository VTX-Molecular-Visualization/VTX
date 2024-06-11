#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_BASIC_FORM_SETTINGS_DIALOG__
#define __VTX_TOOL_TOOLS_MDPREP_UI_BASIC_FORM_SETTINGS_DIALOG__

#include <optional>
#include <variant>

class QWidget;
class QLineEdit;
class QDoubleSpinBox;
class QPushButton;

namespace VTX::Tool::Mdprep::Gateway
{
	struct MdParametersStepData;
	struct MinimizationData;
	struct SystemData;
} // namespace VTX::Tool::Mdprep::Gateway

namespace VTX::Tool::Mdprep::ui
{
	using namespace VTX::Tool::Mdprep::Gateway;

	class MdEngineSpecificFieldPlacer;
	// Class responsible for managing a settings panel for advanced options of an MD step, but from the basic form.
	class BasicFormSettingsDialog
	{
	  public:
		using SupportedData		  = std::variant<MdParametersStepData *, MinimizationData *, SystemData *>;
		BasicFormSettingsDialog() = default;
		BasicFormSettingsDialog(
			QWidget *					p_parent,
			const char *				p_title,
			SupportedData				p_dataFields,
			MdEngineSpecificFieldPlacer p_engineFieldPlacer
		);

		BasicFormSettingsDialog( BasicFormSettingsDialog && )				   = delete;
		BasicFormSettingsDialog( const BasicFormSettingsDialog & )			   = delete;
		BasicFormSettingsDialog & operator=( BasicFormSettingsDialog && )	   = delete;
		BasicFormSettingsDialog & operator=( const BasicFormSettingsDialog & ) = delete;
		~BasicFormSettingsDialog() noexcept;

	  private:
		SupportedData _data;

		struct UiObjects
		{
			QDialog *		 container				  = nullptr;
			QDoubleSpinBox * fieldDt				  = nullptr;
			QLineEdit *		 fieldSaveInterval		  = nullptr;
			QDoubleSpinBox * fieldMinEnergyBreakpoint = nullptr;
			QDoubleSpinBox * fieldMinStepSize		  = nullptr;
			QPushButton *	 buttonApply			  = nullptr;
			QPushButton *	 buttonCancel			  = nullptr;
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
