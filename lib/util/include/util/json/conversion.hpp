#ifndef __VTX_UTIL_JSON_CONVERSION__
#define __VTX_UTIL_JSON_CONVERSION__

#include "_fwd.hpp"
#include "basic_json.hpp"
#include <string>

namespace VTX::Util::JSon
{
	struct Conversion
	{
		static BasicJSon toBasicJSon( const bool p_value );
		static BasicJSon toBasicJSon( const int p_value );
		static BasicJSon toBasicJSon( const size_t p_value );
		static BasicJSon toBasicJSon( const std::string & p_value );
		static BasicJSon toBasicJSon( const Value & p_value );
		static BasicJSon toBasicJSon( const Object & p_value );
		static BasicJSon toBasicJSon( const Array & p_value );
		// static BasicJSon toBasicJSon( const Document & p_value );

	}; // namespace Conversion

	template<typename T>
	concept BasicJSonConcept = requires( T obj ) { Conversion::toBasicJSon( obj ); };
} // namespace VTX::Util::JSon
#endif
