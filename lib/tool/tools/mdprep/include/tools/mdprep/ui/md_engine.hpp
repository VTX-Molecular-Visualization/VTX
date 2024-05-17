#ifndef __VTX_TOOL_TOOLS_MDPREP_UI_MDENGINE__
#define __VTX_TOOL_TOOLS_MDPREP_UI_MDENGINE__

class QLayout;

namespace VTX::Tool::Mdprep::ui
{
	struct CommonFormData;

	// Class responsible for framing md engine form specificities
	class MdEngine
	{
	  public:
		const CommonFormData & get();
	};

} // namespace VTX::Tool::Mdprep::ui

#endif
