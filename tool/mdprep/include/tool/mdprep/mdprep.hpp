#ifndef __VTX_TOOL_TOOLS_MDPREP__
#define __VTX_TOOL_TOOLS_MDPREP__

#include <app/tool/base_tool.hpp>

namespace VTX::Tool::Mdprep
{

	class MdPrep : public App::Tool::BaseTool
	{
	  public:
		MdPrep();
		void registerActions() override;
		void buildUI() override;
	};

} // namespace VTX::Tool::Mdprep
#endif
