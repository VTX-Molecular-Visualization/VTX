#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_FORM_BASIC_FORM_BASIC__
#define __VTX_TOOL_TOOLS_MDPREP_UI_FORM_BASIC_FORM_BASIC__

#include <functional>
#include <optional>
//
#include "tool/mdprep/ui/form_basic/data.hpp"
//
#include "tool/mdprep/ui/form_basic/settings_dialog.hpp"
//
#include "tool/mdprep/ui/form_basic/event_manager.hpp"

class QLineEdit;
class QComboBox;
class QPushButton;
class QLabel;
class QString;
class QWidget;

namespace VTX::Util
{
	class SentryTarget;
}
namespace VTX::Tool::Mdprep::ui
{
	using namespace VTX::Tool::Mdprep::Gateway;

	struct FormLayouts;
	class InputChecker;
	class IonicFields;
	enum class E_FIELD_SECTION;
} // namespace VTX::Tool::Mdprep::ui

namespace VTX::Tool::Mdprep::ui::form_basic
{
	// Class responsible for setting up the form for basic MD parameters
	class FormBasic
	{
	  public:
		FormBasic() = delete;
		FormBasic(
			QWidget * p_container,
			SpecificFieldsPlacerCallback,
			const MdParameters & p_defaults,
			ReportManager &		 p_reportManager
		);

		/**
		 * @brief Change information related to the current engine.
		 * @param Data
		 */
		void update( const EngineSpecificCommonInformation & p_data ) noexcept;

		void get( MdParameters & ) const noexcept;
		void get( FormLayouts & ) const noexcept;

	  private:
		std::array<VTX::UI::QT::Util::ObjectOwnership, 2> _toDeleteObjects;

		// Data structure that put all ui entities together
		struct UiData
		{
			QWidget * _parent	 = nullptr;
			QWidget * _container = nullptr;

			QFormLayout * _layoutFieldEngineSpecific = nullptr;
			QFormLayout * _layoutIons				 = nullptr;

			QWidget *	  _labelSystemCheckMsgContainer = nullptr;
			QVBoxLayout * _layoutSystemCheckMsg			= nullptr;
			QPushButton * _buttonSystemSettings			= nullptr;
			QComboBox *	  _fieldForceFieldBio			= nullptr;
			QComboBox *	  _fieldForceFieldChem			= nullptr;

			QLineEdit *	  _fieldStepNumberMinimization		  = nullptr;
			QPushButton * _buttonMinimizationSettings		  = nullptr;
			QLineEdit *	  _fieldDurationValueEquilibrationNvt = nullptr;
			QComboBox *	  _fieldDurationUnitEquilibrationNvt  = nullptr;
			QPushButton * _buttonEquilibrationNvtSettings	  = nullptr;
			QLineEdit *	  _fieldDurationValueEquilibrationNpt = nullptr;
			QComboBox *	  _fieldDurationUnitEquilibrationNpt  = nullptr;
			QPushButton * _buttonEquilibrationNptSettings	  = nullptr;
			QLineEdit *	  _fieldDurationValueProduction		  = nullptr;
			QComboBox *	  _fieldDurationUnitProduction		  = nullptr;
			QPushButton * _buttonProductionSettings			  = nullptr;
			QPushButton * _buttonInputCheck					  = nullptr;
		} _uiObjects;
		std::unique_ptr<Data> _data = nullptr;
		IonicFields			  _fieldsIons;
		EventManager		  _eventManager;

		void _createAndPlaceUiItems( QWidget * p_container ) noexcept;
		void _loadValues( const MdParameters & p_defaults ) noexcept;
		void _setupSlots() noexcept;
	};
} // namespace VTX::Tool::Mdprep::ui::form_basic
#endif
