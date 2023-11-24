#include "util/json/object.hpp"

namespace VTX::Util::JSon
{
	Object::Object() {}
	Object::Object( const std::string p_key, const BasicJSon & p_value ) { _map[ p_key ] = p_value; }
	Object::Object( const Field & p_field ) { appendField( p_field ); }
	Object::Object( const std::initializer_list<Field> & p_init )
	{
		for ( const Field & field : p_init )
		{
			appendField( field );
		}
	}

	std::map<std::string, BasicJSon>::const_iterator Object::begin() const { return _map.begin(); }
	std::map<std::string, BasicJSon>::const_iterator Object::end() const { return _map.end(); }

	void Object::appendField( const Field & p_field ) { _map[ p_field.first ] = p_field.second; }

} // namespace VTX::Util::JSon
