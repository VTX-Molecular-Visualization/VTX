#ifndef __VTX_UI_CONCEPTS__
#define __VTX_UI_CONCEPTS__

#include "descriptors.hpp"
#include <string>

namespace VTX::UI
{

	template<typename MW>
	concept ConceptMainWindow =
		// std::is_base_of<BaseMainWindow, MW>::value &&
		requires( MW p_mw, MenuAction p_menuAction, ToolBarAction p_toolBarAction ) {
			{ p_mw.build() } -> std::same_as<void>;
			{ p_mw.addMenuAction( p_menuAction ) } -> std::same_as<void>;
			{ p_mw.addToolBarAction( p_toolBarAction ) } -> std::same_as<void>;
		};

} // namespace VTX::UI

#endif
