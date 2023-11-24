#include "util/json/conversion.hpp"
#include "util/json/document.hpp"
#include "util/json/object.hpp"
#include "util/json/value.hpp"

namespace VTX::Util::JSon
{
	BasicJSon Conversion::toBasicJSon( const bool p_value ) { return BasicJSon( Value( p_value ) ); }
	BasicJSon Conversion::toBasicJSon( const int p_value ) { return BasicJSon( Value( p_value ) ); }
	BasicJSon Conversion::toBasicJSon( const size_t p_value ) { return BasicJSon( Value( p_value ) ); }
	BasicJSon Conversion::toBasicJSon( const std::string & p_value ) { return BasicJSon( Value( p_value ) ); }
	BasicJSon Conversion::toBasicJSon( const Value & p_value ) { return BasicJSon( p_value ); }
	BasicJSon Conversion::toBasicJSon( const Object & p_value ) { return BasicJSon( p_value ); }
	BasicJSon Conversion::toBasicJSon( const Array & p_value ) { return BasicJSon( p_value ); }
	// BasicJSon toBasicJSon( const Document & p_value ) { return BasicJSon( p_value ); }
} // namespace VTX::Util::JSon
