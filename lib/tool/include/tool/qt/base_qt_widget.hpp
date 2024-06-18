#ifndef __VTX_UI_QT_BASE_QT_TOOL__
#define __VTX_UI_QT_BASE_QT_TOOL__

#include "tool/core/base_ui_tool.hpp"
// #include <QObject>

namespace VTX::UI::QT
{
	class BaseQtTool : public VTX::UI::Core::BaseVTXUITool //, public QObject
	{
	  public:
		BaseQtTool();
	};
} // namespace VTX::UI::QT

#endif
