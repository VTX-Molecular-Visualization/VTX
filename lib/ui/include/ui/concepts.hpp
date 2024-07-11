#ifndef __VTX_UI_CONCEPTS__
#define __VTX_UI_CONCEPTS__

#include "descriptors.hpp"
#include <string>

namespace VTX::UI
{
	template<typename MW>
	concept ConceptMainWindow = requires( MW p_mw, WidgetId p_id, Action p_action ) {
		{ p_mw.build() } -> std::same_as<void>;
		{ p_mw.addMenuAction( p_id, p_action ) } -> std::same_as<void>;
		{ p_mw.addToolBarAction( p_id, p_action ) } -> std::same_as<void>;
	};

} // namespace VTX::UI

#endif
