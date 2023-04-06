#ifndef __VTX_UI_CORE_TOOL_REGISTRY__
#define __VTX_UI_CORE_TOOL_REGISTRY__

#include "base_ui_tool.hpp"
#include "layout_descriptor.hpp"
#include <map>
#include <type_traits>

namespace VTX::UI::Core
{
	template<typename T>
	BaseVTXUITool * createT()
	{
		return new T();
	}

	class ToolRegistry
	{
	  protected:
		using map_type = std::map<ToolIdentifier, BaseVTXUITool * (*)()>;

	  public:
		static BaseVTXUITool * createInstance( const ToolIdentifier & p_identifier )
		{
			map_type::iterator it = map.find( p_identifier );

			if ( it == getMap().end() )
				return nullptr;

			return it->second();
		}

		template<typename T>
		static void registerTool( const ToolIdentifier & p_identifier )
		{
			getMap().insert( std::make_pair( p_identifier, &createT<T> ) );
		}

	  protected:
		static map_type & getMap() { return map; }

	  private:
		inline static map_type map = map_type();
	};

} // namespace VTX::UI::Core

#endif
