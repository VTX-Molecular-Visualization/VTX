#ifndef VTX_TOOL_MDPREP_MDPREP_HPP
#define VTX_TOOL_MDPREP_MDPREP_HPP

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
