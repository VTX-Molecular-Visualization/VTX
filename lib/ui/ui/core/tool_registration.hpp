#ifndef __VTX_UI_CORE_TOOL_REGISTRATION__
#define __VTX_UI_CORE_TOOL_REGISTRATION__

#include "base_ui_tool.hpp"
#include "tool_registry.hpp"

namespace VTX
{
#define REGISTER_TOOL( TYPE, NAME ) \
	inline static const VTX::UI::Core::Registration<TYPE> reg = VTX::UI::Core::Registration<TYPE>( NAME )
} // namespace VTX

namespace VTX::UI::Core
{

	template<typename T>
	class Registration
	{
	  public:
		Registration( ToolIdentifier const & p_identifier ) { ToolRegistry::registerTool<T>( p_identifier ); }
	};
} // namespace VTX::UI::Core

#endif
