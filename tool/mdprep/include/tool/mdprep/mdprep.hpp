#ifndef __VTX_TOOL_TOOLS_MDPREP__
#define __VTX_TOOL_TOOLS_MDPREP__

#include <app/tool/base_tool.hpp>

namespace VTX::Tool::Mdprep
{

	class MdPrep : public App::Tool::BaseTool
	{
	  public:
		virtual void init() override;
		virtual void onAppStart() override;
		virtual void createUI() override;
		virtual void onAppStop() override;
	};

} // namespace VTX::Tool::Mdprep
#endif
