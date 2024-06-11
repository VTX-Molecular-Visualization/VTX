#ifndef __VTX_UI_CORE_TOOL_REGISTRY__
#define __VTX_UI_CORE_TOOL_REGISTRY__

#include "base_ui_tool.hpp"
#include "layout_descriptor.hpp"
#include <app/core/collection.hpp>
#include <functional>
#include <map>
#include <memory>
#include <type_traits>

namespace VTX::UI::Core
{
	template<typename T>
	std::unique_ptr<BaseVTXUITool> createT()
	{
		return std::make_unique<T>();
	}

	class ToolRegistry
	{
	  public:
		template<typename T2>
		class Registration
		{
		  public:
			Registration( const ToolIdentifier & p_identifier ) { registerTool<T2>( p_identifier ); }
		};

	  protected:
		using map_type = std::map<ToolIdentifier, std::function<std::unique_ptr<BaseVTXUITool>()>>;

	  public:
		static std::unique_ptr<BaseVTXUITool> createInstance( const ToolIdentifier & p_identifier )
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

		static bool exists( const ToolIdentifier & p_identifier ) { return map.find( p_identifier ) != map.end(); }

	  protected:
		static map_type & getMap() { return map; }

	  private:
		inline static map_type map = map_type();
	};
} // namespace VTX::UI::Core

#endif
