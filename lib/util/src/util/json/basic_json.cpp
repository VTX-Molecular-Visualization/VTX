#include "util/json/basic_json.hpp"
#include "util/json/array.hpp"
#include "util/json/document.hpp"
#include "util/json/object.hpp"

namespace VTX::Util::JSon
{
	BasicJSon::BasicJSon() : _type( EnumType::Unknown ), _value() {}
	BasicJSon::BasicJSon( const bool p_value ) : _type( EnumType::Bool ), _value( p_value ) {}
	BasicJSon::BasicJSon( const int p_value ) : _type( EnumType::Numeral ), _value( size_t( p_value ) ) {}
	BasicJSon::BasicJSon( const size_t p_value ) : _type( EnumType::Numeral ), _value( p_value ) {}
	BasicJSon::BasicJSon( const float p_value ) : _type( EnumType::Floating ), _value( p_value ) {}
	BasicJSon::BasicJSon( const char * p_value ) : _type( EnumType::String ), _value( std::string( p_value ) ) {}
	BasicJSon::BasicJSon( const std::string & p_value ) : _type( EnumType::String ), _value( p_value ) {}
	BasicJSon::BasicJSon( const Array & p_value ) : _type( EnumType::Array ), _value( &p_value ) {}
	BasicJSon::BasicJSon( const Object & p_value ) : _type( EnumType::Object ), _value( &p_value ) {}
	BasicJSon::BasicJSon( const Document & p_value ) : _type( EnumType::Document ), _value( &p_value ) {}

	BasicJSon::BasicJSon( const BasicJSon & p_source ) = default;

	BasicJSon::BasicJSon( std::initializer_list<BasicJSon> p_init )
	{
		if ( p_init.size() == 0 )
		{
			_type = EnumType::Unknown;
		}
		else if ( _isDescribingObject( p_init ) )
		{
			_type = EnumType::Object;

			Object * obj = new Object();
			_fillObjectFieldsFromVector( *obj, p_init );
			_value = obj;
		}
		else
		{
			_type  = EnumType::Array;
			_value = new Array( std::forward<std::initializer_list<BasicJSon>>( p_init ) );
		}
	}

	bool				BasicJSon::getBool() const { return std::get<bool>( _value ); }
	size_t				BasicJSon::getNumberInteger() const { return std::get<size_t>( _value ); }
	float				BasicJSon::getNumberFloat() const { return std::get<float>( _value ); }
	const std::string & BasicJSon::getString() const { return std::get<std::string>( _value ); }
	const Array &		BasicJSon::getArray() const { return *std::get<const Array *>( _value ); }
	const Object &		BasicJSon::getObject() const { return *std::get<const Object *>( _value ); }
	const Document &	BasicJSon::getDocument() const { return *std::get<const Document *>( _value ); }

	bool BasicJSon::_isDescribingObject( const std::initializer_list<BasicJSon> & p_data )
	{
		for ( const BasicJSon & potentialField : p_data )
		{
			if ( !_isObjectField( potentialField ) )
				return false;
		}

		return true;
	}
	bool BasicJSon::_isObjectField( const BasicJSon & p_potentialField )
	{
		if ( p_potentialField.getType() != BasicJSon::EnumType::Array )
			return false;

		const Array & potentialFieldArray = p_potentialField.getArray();

		return potentialFieldArray.size() == 2 && potentialFieldArray.begin()->getType() == BasicJSon::EnumType::String;
	}

	void BasicJSon::_fillObjectFieldsFromVector( Object & p_obj, const std::initializer_list<BasicJSon> & p_vector )
	{
		for ( const BasicJSon & json : p_vector )
		{
			const Array & arrayField = json.getArray();
			p_obj.appendField( arrayField[ 0 ].getString(), arrayField[ 1 ] );
		}
	}

} // namespace VTX::Util::JSon
