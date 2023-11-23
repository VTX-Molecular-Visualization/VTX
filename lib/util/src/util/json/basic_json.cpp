#include "util/json/basic_json.hpp"
#include "util/json/array.hpp"
#include "util/json/document.hpp"
#include "util/json/object.hpp"
#include "util/json/value.hpp"

namespace VTX::Util::JSon
{
	BasicJSon::BasicJSon() : _type( EnumType::Unknown ), _value() {}
	BasicJSon::BasicJSon( const Value & p_value ) : _type( EnumType::BaseValue ), _value( new Value( p_value ) ) {}
	BasicJSon::BasicJSon( const Array & p_value ) : _type( EnumType::Array ), _value( new Array( p_value ) ) {}
	BasicJSon::BasicJSon( const Object & p_value ) : _type( EnumType::Object ), _value( new Object( p_value ) ) {}
	BasicJSon::BasicJSon( const Document & p_value ) : _type( EnumType::Document ), _value( new Document( p_value ) ) {}

	BasicJSon::BasicJSon( const BasicJSon & p_source ) = default;

	BasicJSon::BasicJSon( std::initializer_list<BasicJSon> p_init )
	{
		if ( p_init.size() == 0 )
		{
			_type = EnumType::Unknown;
		}
		else if ( p_init.size() == 1 )
		{
			BasicJSon( *p_init.begin() );
		}
		else if ( _isObjectField( p_init ) )
		{
			const std::vector<BasicJSon> vecInit = p_init;
			const Object::Field			 field	 = { vecInit[ 0 ].getValue().getString(), vecInit[ 1 ] };

			_type  = EnumType::Object;
			_value = new Object( { field } );
		}
		else
		{
			_type  = EnumType::Array;
			_value = new Array( std::forward<std::initializer_list<BasicJSon>>( p_init ) );
		}
	}

	const Value &	 BasicJSon::getValue() const { return *std::get<Value *>( _value ); }
	const Array &	 BasicJSon::getArray() const { return *std::get<Array *>( _value ); }
	const Object &	 BasicJSon::getObject() const { return *std::get<Object *>( _value ); }
	const Document & BasicJSon::getDocument() const { return *std::get<Document *>( _value ); }

	void BasicJSon::setValue( const Array & p_value )
	{
		_type  = EnumType::Array;
		_value = new Array( p_value );
	}
	void BasicJSon::setValue( const Value & p_value )
	{
		_type  = EnumType::BaseValue;
		_value = new Value( p_value );
	}
	void BasicJSon::setValue( const Object & p_value )
	{
		_type  = EnumType::Object;
		_value = new Object( p_value );
	}
	void BasicJSon::setValue( const Document & p_value )
	{
		_type  = EnumType::Document;
		_value = new Document( p_value );
	}

	bool BasicJSon::_isObjectField( const std::vector<BasicJSon> & p_init ) const
	{
		if ( p_init.size() != 2 )
			return false;

		const BasicJSon & firstItem = *p_init.begin();

		return firstItem.getType() == BasicJSon::EnumType::BaseValue
			   && firstItem.getValue().getType() == Value::EnumType::String;
	}

} // namespace VTX::Util::JSon
