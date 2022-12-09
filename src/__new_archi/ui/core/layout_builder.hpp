#ifndef __VTX_UI_CORE_LAYOUT_BUILDER__
#define __VTX_UI_CORE_LAYOUT_BUILDER__

#include "__new_archi/ui/core/layout_descriptor.hpp"
#include <vector>

namespace VTX::UI::Core
{
	class LayoutBuilder
	{
	  public:
		void build( const LayoutDescriptor & p_layoutDescriptor ) const;

	  private:
		void _instantiateTool( const ToolDescriptor & p_toolDescriptor ) const;
	};

} // namespace VTX::UI::Core

#endif
