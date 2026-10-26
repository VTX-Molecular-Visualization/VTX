#ifndef VTX_TOOL_MDPREP_UI_FORM_SWITCH_BUTTON_HPP
#define VTX_TOOL_MDPREP_UI_FORM_SWITCH_BUTTON_HPP

#include <array>
#include <functional>
#include <string>

class QWidget;

namespace VTX::Tool::Mdprep::ui
{
	struct FormLayouts;

	// Class responsible for settings up the MD UI with respect of basic VS advanced MD parameters
	class FormSwitchButton
	{
	  public:
		enum class E_FORM_MODE : uint8_t
		{
			none,
			basic,
			advanced,
			COUNT
		};
		using Callback	   = std::function<void()>;
		FormSwitchButton() = default;

		// Do the work. layouts won't be nullptr after that.
		void setupUi( QLayout *, const E_FORM_MODE & );

		void subscribeBasicSwitch( Callback );
		void subscribeAdvancedSwitch( Callback );
		void switchFormMode();

	  private:
		E_FORM_MODE	  _mode				= E_FORM_MODE::none;
		QPushButton * _buttonToBasic	= nullptr;
		QPushButton * _buttonToAdvanced = nullptr;
		Callback	  _switchToBasic;
		Callback	  _switchToAdvanced;

		void _changeModeBasic();
		void _changeModeAdvanced();
		void _uiUncheckAllButtons();
	};

} // namespace VTX::Tool::Mdprep::ui

#endif
