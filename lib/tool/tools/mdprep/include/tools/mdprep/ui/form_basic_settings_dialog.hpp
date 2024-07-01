#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_FORM_BASIC_SETTINGS_DIALOG__
#define __VTX_TOOL_TOOLS_MDPREP_UI_FORM_BASIC_SETTINGS_DIALOG__

#include <optional>
#include <variant>

class QWidget;
class QLineEdit;
class QDoubleSpinBox;
class QPushButton;

namespace VTX::Tool::Mdprep::Gateway
{
	struct MdParametersStepData;
	struct EngineSpecificCommonInformation;
	struct MinimizationData;
	struct SystemData;
} // namespace VTX::Tool::Mdprep::Gateway

namespace VTX::Tool::Mdprep::ui
{
	using namespace VTX::Tool::Mdprep::Gateway;

	class MdEngineSpecificFieldPlacer;
	// Class responsible for managing a settings panel for advanced options of an MD step, but from the basic form.
	class FormBasicSettingsDialog
	{
	  public:
		using SupportedData		  = std::variant<MdParametersStepData *, MinimizationData *, SystemData *>;
		FormBasicSettingsDialog() = default;
		FormBasicSettingsDialog(
			QWidget *												p_parent,
			const char *											p_title,
			SupportedData											p_dataFields,
			MdEngineSpecificFieldPlacer								p_engineFieldPlacer,
			std::optional<Gateway::EngineSpecificCommonInformation> p_engineInfo = std::nullopt
		);

		FormBasicSettingsDialog( const FormBasicSettingsDialog & )			   = delete;
		FormBasicSettingsDialog & operator=( const FormBasicSettingsDialog & ) = delete;
		FormBasicSettingsDialog( FormBasicSettingsDialog && )				   = default;
		FormBasicSettingsDialog & operator=( FormBasicSettingsDialog && )	   = default;
		~FormBasicSettingsDialog() noexcept;

	  private:
		SupportedData _data;

		struct UiData
		{
			QDialog *		 container				  = nullptr;
			QDoubleSpinBox * fieldDt				  = nullptr;
			QLineEdit *		 fieldSaveInterval		  = nullptr;
			QDoubleSpinBox * fieldMinEnergyBreakpoint = nullptr;
			QDoubleSpinBox * fieldMinStepSize		  = nullptr;
			QPushButton *	 buttonApply			  = nullptr;
			QPushButton *	 buttonCancel			  = nullptr;
			QComboBox *		 fieldForceFieldBio		  = nullptr;
			QComboBox *		 fieldForceFieldChem	  = nullptr;
			QComboBox *		 fieldBoxShape			  = nullptr;

			std::optional<Gateway::EngineSpecificCommonInformation> engineInfo;
		};
		UiData						_uiObjects;
		MdEngineSpecificFieldPlacer _engineFieldPlacer;

		void _setupUi( QWidget * p_parent, const char * p_title );
		void _setupSlots() noexcept;
		void _applyChanges() noexcept;
		void _cancelChanges() noexcept;
	};
} // namespace VTX::Tool::Mdprep::ui

#endif
