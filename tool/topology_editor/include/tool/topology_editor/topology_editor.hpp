#ifndef __VTX_TOOL_TOPOLOGY_EDITOR_TOPOLOGY_EDITOR__
#define __VTX_TOOL_TOPOLOGY_EDITOR_TOPOLOGY_EDITOR__

#include <app/tool/base_tool.hpp>

namespace VTX::Tool::TopologyEditor
{
	class TopologyEditor : public App::Tool::BaseTool
	{
	  public:
		TopologyEditor();
		void registerActions() override;
		void buildUI() override;
	};
} // namespace VTX::Tool::TopologyEditor

#endif
