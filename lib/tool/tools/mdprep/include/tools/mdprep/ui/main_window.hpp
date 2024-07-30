#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_MAINWINDOW__
#define __VTX_TOOL_TOOLS_MDPREP_UI_MAINWINDOW__

#include <array>
#include <string>

class QWidget;

namespace VTX::Tool::Mdprep::ui
{

	class MdEngineFieldPlacer;
	struct FormLayouts;

	// Class responsible for settings up the MD UI with respect of basic VS advanced MD parameters
	class MdFieldsOrganizer
	{
	  public:
		enum class E_FORM_MODE
		{
			basic,
			advanced,
			COUNT
		};
		QWidget * containerParamBasic	 = nullptr;
		QWidget * containerParamAdvanced = nullptr;

		// Construct only for data allocation
		MdFieldsOrganizer() = default;

		// Do the work. layouts won't be nullptr after that.
		void setupUi( QLayout *, const E_FORM_MODE & ) noexcept;

		void switchFormMode() noexcept;

	  private:
		E_FORM_MODE	  _mode				= E_FORM_MODE::basic;
		QPushButton * _buttonViewSwitch = nullptr;

		void _changeModeBasic() noexcept;
		void _changeModeAdvanced() noexcept;
	};

} // namespace VTX::Tool::Mdprep::ui

#endif