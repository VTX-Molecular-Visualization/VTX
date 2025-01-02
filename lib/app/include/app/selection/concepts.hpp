#ifndef __VTX_APP_SELECTION_CONCEPT__
#define __VTX_APP_SELECTION_CONCEPT__

#include "selection_data.hpp"
#include <concepts>

namespace VTX::App::Selection
{
	template<typename T>
	concept SelectionDataConcept = std::derived_from<T, SelectionData>;

} // namespace VTX::App::Selection

#endif
