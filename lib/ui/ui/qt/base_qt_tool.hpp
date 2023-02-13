#ifndef __VTX_UI_QT_BASE_QT_TOOL__
#define __VTX_UI_QT_BASE_QT_TOOL__

#include <QObject>
#include <ui/core/base_ui_tool.hpp>
#include <ui/core/tool_registration.hpp>

namespace VTX::UI::QT
{
	class BaseQtTool : public VTX::UI::Core::BaseVTXUITool, public QObject
	{
	  public:
		BaseQtTool();
	};
} // namespace VTX::UI::QT

#endif
