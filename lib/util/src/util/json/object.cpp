#include "util/json/object.hpp"

namespace VTX::Util::JSon
{
	Object::Object() {}
	Object::Object( const std::initializer_list<Field> & p_init )
	{
		for ( const Field & field : p_init )
		{
			appendField( field );
		}
	}

	std::map<std::string, BasicJSon>::const_iterator Object::begin() const { return _map.begin(); }
	std::map<std::string, BasicJSon>::const_iterator Object::end() const { return _map.end(); }

	void Object::appendField( const Field & p_field ) { _map.emplace( p_field.first, p_field.second ); }
	void Object::appendField( const std::string & p_key, const BasicJSon & p_value ) { _map.emplace( p_key, p_value ); }

	BasicJSon & Object::operator[]( const std::string & p_key ) { return _map[ p_key ]; }
	const BasicJSon & Object::operator[]( const std::string & p_key ) const { return _map.at( p_key ); }

	bool Object::contains( const std::string & p_key ) const { return _map.contains( p_key ); }

} // namespace VTX::Util::JSon
