#ifndef __VTX_TOOL_QT_BASE_QT_TOOL__
#define __VTX_TOOL_QT_BASE_QT_TOOL__

#include <QObject>
#include <ui/core/base_ui_tool.hpp>
#include <ui/core/tool_registration.hpp>

namespace VTX::Tool::QT
{
	class BaseQtTool : public VTX::UI::Core::BaseVTXUITool, public QObject
	{
	  public:
		BaseQtTool();
	};
} // namespace VTX::Tool::QT

#endif
