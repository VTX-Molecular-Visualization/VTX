#ifndef __VTX_UTIL_JSON_CONCEPTS__
#define __VTX_UTIL_JSON_CONCEPTS__

#include "_fwd.hpp"
#include "basic_json.hpp"
#include <concepts>

namespace VTX::Util::JSon
{
	template<typename T>
	concept BasicJSonConcept = std::convertible_to<T, BasicJSon>;

} // namespace VTX::Util::JSon
#endif
