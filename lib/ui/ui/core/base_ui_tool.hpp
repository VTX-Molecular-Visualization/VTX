#ifndef __VTX_UI_CORE_BASE_UI_TOOL__
#define __VTX_UI_CORE_BASE_UI_TOOL__

#include "layout_descriptor.hpp"
#include <string>

namespace VTX::UI::Core
{
	class BaseVTXUITool
	{
	  public:
		virtual void instantiateTool() = 0;

		void setLayoutData( const ToolLayoutData & p_layoutData ) { _layoutData = p_layoutData; }

	  protected:
		const ToolLayoutData & getLayoutData() const { return _layoutData; }

	  private:
		ToolLayoutData _layoutData;
	};
} // namespace VTX::UI::Core

#endif
