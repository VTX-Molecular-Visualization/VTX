#ifndef __VTX_UI_TOOL_SCENE_WINDOW__
#define __VTX_UI_TOOL_SCENE_WINDOW__

#include "ui/core/tool_registry.hpp"
#include "ui/qt/base_qt_tool.hpp"
#include <string>

namespace VTX::UI::QT::Tool
{
	class SceneTool : public BaseQtTool
	{
	  public:
		inline static const UI::Core::ToolIdentifier TOOL_KEY = "SCENE_TOOL_KEY";

	  private:
		inline static const UI::Core::ToolRegistry::Registration<SceneTool> _reg { TOOL_KEY };

	  public:
		inline static const std::string SCENE_PANEL_KEY = "SCENE_PANEL_KEY";

	  public:
		SceneTool();
		void instantiateTool() override;
	};
} // namespace VTX::UI::QT::Tool

#endif
