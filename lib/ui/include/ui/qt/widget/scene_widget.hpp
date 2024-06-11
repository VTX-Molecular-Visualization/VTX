#ifndef __VTX_UI_TOOL_SCENE_WINDOW__
#define __VTX_UI_TOOL_SCENE_WINDOW__

#include "ui/core/tool_registry.hpp"
#include "ui/qt/base_qt_widget.hpp"
#include <string>

namespace VTX::UI::QT::Widget
{
	class SceneWidget : public BaseQtTool
	{
	  public:
		inline static const UI::Core::ToolIdentifier TOOL_KEY = "SCENE_TOOL_KEY";

	  private:
		inline static const UI::Core::ToolRegistry::Registration<SceneWidget> _reg { TOOL_KEY };

	  public:
		inline static const std::string SCENE_PANEL_KEY = "SCENE_PANEL_KEY";

	  public:
		SceneWidget();
		void instantiateTool() override;
	};
} // namespace VTX::UI::QT::Widget

#endif
