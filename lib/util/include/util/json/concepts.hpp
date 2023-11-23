#ifndef __VTX_UTIL_JSON_CONCEPTS__
#define __VTX_UTIL_JSON_CONCEPTS__

#include "util/json/array.hpp"
#include "util/json/object.hpp"
#include "util/json/value.hpp"
#include <concepts>

namespace VTX::Util::JSon
{

	template<typename T>
	concept ArrayConcept
		= std::convertible_to<T, Util::JSon::Array> || requires( T obj ) { new Util::JSon::Array( obj ); };

	template<typename T>
	concept ObjectConcept
		= std::convertible_to<T, Util::JSon::Object> || requires( T obj ) { new Util::JSon::Object( obj ); };

} // namespace VTX::Util::JSon

#endif
