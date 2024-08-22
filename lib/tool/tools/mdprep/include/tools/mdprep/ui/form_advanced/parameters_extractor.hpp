#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_FORM_ADVANCED_PARAMETERS_EXTRACTOR__
#define __VTX_TOOL_TOOLS_MDPREP_UI_FORM_ADVANCED_PARAMETERS_EXTRACTOR__

namespace VTX::Tool::Mdprep::ui::Gateway
{
	struct MdParameters;
}
namespace VTX::Tool::Mdprep::ui::form_advanced
{
	struct UiData;

	class ParametersExtractor
	{
		ParametersExtractor() = delete;
		ParametersExtractor( UiData &, Gateway::MdParameters & );

	  private:
		UiData * _uiData = nullptr;
		Gateway::MdParameters * _parameters
	};

} // namespace VTX::Tool::Mdprep::ui::form_advanced

#endif
