#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_SCREEN_FORMS__
#define __VTX_TOOL_TOOLS_MDPREP_UI_SCREEN_FORMS__

namespace VTX::Tool::Mdprep::Gateway
{
	struct MdParameters;
} // namespace VTX::Tool::Mdprep::Gateway
namespace VTX::Tool::Mdprep::ui
{

	// Class responsible for signaling that a form is validated
	class ValidationSignaler
	{
	};

	// Class responsible for setting up the form screens, allowing user to fill it and start the processing
	class ScreenForms
	{
	  public:
		ScreenForms() = delete;
		ScreenForms( Gateway::MdParameters &, ValidationSignaler );

	  private:
		Gateway::MdParameters * _dataPtr = nullptr;
		ValidationSignaler		_validationSignaler;
	};
} // namespace VTX::Tool::Mdprep::ui

#endif
