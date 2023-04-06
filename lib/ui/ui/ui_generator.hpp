#ifndef __VTX_UI_CORE_VTX_APP__
#define __VTX_UI_CORE_VTX_APP__

#include "core/base_ui_application.hpp"

// TODO Define that in cmake
#define VTX_UI_STYLE_COMMAND_LINE 0
#define VTX_UI_STYLE_QT 1

#define VTX_UI_STYLE VTX_UI_STYLE_QT

namespace VTX::UI
{
	class UIGenerator
	{
	  public:
		static Core::BaseUIApplication * createUI();
	};
} // namespace VTX::UI

#endif
