#include "util/json/value.hpp"
#include "util/json/basic_json.hpp"

namespace VTX::Util::JSon
{
	Value::Value( const bool p_value ) : _value( p_value ), _type( EnumType::Bool ) {}
	Value::Value( const int p_value ) : _value( size_t( p_value ) ), _type( EnumType::Numeral ) {}
	Value::Value( const size_t p_value ) : _value( p_value ), _type( EnumType::Numeral ) {}
	Value::Value( const float p_value ) : _value( p_value ), _type( EnumType::Floating ) {}
	Value::Value( const char * p_value ) : _value( std::string( p_value ) ), _type( EnumType::String ) {}
	Value::Value( const std::string & p_value ) : _value( p_value ), _type( EnumType::String ) {}

	bool				Value::getBool() const { return std::get<bool>( _value ); }
	size_t				Value::getNumberInteger() const { return std::get<size_t>( _value ); }
	float				Value::getNumberFloat() const { return std::get<float>( _value ); }
	const std::string & Value::getString() const { return std::get<std::string>( _value ); }

} // namespace VTX::Util::JSon
