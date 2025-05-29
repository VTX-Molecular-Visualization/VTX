#include "util/json/basic_json.hpp"
#include "util/json/array.hpp"
#include "util/json/document.hpp"
#include "util/json/object.hpp"

namespace VTX::Util::JSon
{
	BasicJSon::BasicJSon() : _type( EnumType::Unknown ), _value() {}
	BasicJSon::BasicJSon( const bool p_value ) : _type( EnumType::Bool ), _value( size_t( p_value ) ) {}
	BasicJSon::BasicJSon( const size_t p_value ) : _type( EnumType::Integral ), _value( p_value ) {}
	BasicJSon::BasicJSon( const double p_value ) : _type( EnumType::FloatingPoint ), _value( p_value ) {}
	BasicJSon::BasicJSon( const char * p_value ) : _type( EnumType::String ), _value( std::string( p_value ) ) {}
	BasicJSon::BasicJSon( const std::string & p_value ) : _type( EnumType::String ), _value( p_value ) {}
	BasicJSon::BasicJSon( const Array & p_value ) :
		_type( EnumType::Array ), _value( std::make_unique<Array>( p_value ) )
	{
	}
	BasicJSon::BasicJSon( const Object & p_value ) :
		_type( EnumType::Object ), _value( std::make_unique<Object>( p_value ) )
	{
	}
	BasicJSon::BasicJSon( const Document & p_value ) :
		_type( EnumType::Document ), _value( std::make_unique<Document>( p_value ) )
	{
	}
	BasicJSon::BasicJSon( std::unique_ptr<Array> p_value ) : _type( EnumType::Array ), _value( std::move( p_value ) ) {}
	BasicJSon::BasicJSon( std::unique_ptr<Object> p_value ) : _type( EnumType::Object ), _value( std::move( p_value ) )
	{
	}
	BasicJSon::BasicJSon( std::unique_ptr<Document> p_value ) :
		_type( EnumType::Document ), _value( std::move( p_value ) )
	{
	}

	BasicJSon::BasicJSon( const BasicJSon & p_source ) { _copyFrom( p_source ); };
	BasicJSon::BasicJSon( std::initializer_list<BasicJSon> p_init )
	{
		if ( p_init.size() == 0 )
		{
			_type = EnumType::Unknown;
		}
		else if ( _isDescribingObject( p_init ) )
		{
			_type = EnumType::Object;

			std::unique_ptr<Object> obj = std::make_unique<Object>();
			_fillObjectFieldsFromVector( *obj, p_init );

			_value = std::move( obj );
		}
		else
		{
			_type  = EnumType::Array;
			_value = std::make_unique<Array>( p_init );
		}
	}

	BasicJSon::~BasicJSon() = default;

	BasicJSon & BasicJSon::operator=( const BasicJSon & p_source )
	{
		_copyFrom( p_source );
		return *this;
	}

	bool				BasicJSon::getBool() const { return std::get<size_t>( _value ); }
	size_t				BasicJSon::getIntegral() const { return std::get<size_t>( _value ); }
	double				BasicJSon::getFloatingPoint() const { return std::get<double>( _value ); }
	const std::string & BasicJSon::getString() const { return std::get<std::string>( _value ); }
	const Array &		BasicJSon::getArray() const { return *std::get<std::unique_ptr<Array>>( _value ); }
	const Object &		BasicJSon::getObject() const { return *std::get<std::unique_ptr<Object>>( _value ); }
	const Document &	BasicJSon::getDocument() const { return *std::get<std::unique_ptr<Document>>( _value ); }

	Array &	 BasicJSon::getArrayEditable() { return *std::get<std::unique_ptr<Array>>( _value ); }
	Object & BasicJSon::getObjectEditable() { return *std::get<std::unique_ptr<Object>>( _value ); }

	BasicJSon::operator const Array &() const { return getArray(); }
	BasicJSon::operator Array &() { return getArrayEditable(); }
	BasicJSon::operator const Object &() const { return getObject(); }
	BasicJSon::operator Object &() { return getObjectEditable(); }

	const BasicJSon & BasicJSon::operator[]( const std::string & p_key ) const { return getObject()[ p_key ]; }
	BasicJSon & BasicJSon::operator[]( const std::string & p_key ) { return getObjectEditable()[ p_key ]; }
	const BasicJSon & BasicJSon::operator[]( const size_t & p_index ) const { return getArray()[ p_index ]; }
	BasicJSon & BasicJSon::operator[]( const size_t & p_index ) { return getArrayEditable()[ p_index ]; }

	bool BasicJSon::contains( const std::string & p_key ) const { return getObject().contains( p_key ); }

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

	void BasicJSon::_copyFrom( const BasicJSon & p_source )
	{
		_type = p_source._type;

		switch ( _type )
		{
		case EnumType::Bool: _value = p_source.getBool(); break;
		case EnumType::Integral: _value = p_source.getIntegral(); break;
		case EnumType::FloatingPoint: _value = p_source.getFloatingPoint(); break;
		case EnumType::String: _value = p_source.getString(); break;
		case EnumType::Array: _value = std::make_unique<Array>( p_source.getArray() ); break;
		case EnumType::Object: _value = std::make_unique<Object>( p_source.getObject() ); break;
		case EnumType::Document: _value = std::make_unique<Document>( p_source.getDocument() ); break;
		case EnumType::Unknown:
		default: break;
		}
	}

} // namespace VTX::Util::JSon
