#include "util/json/document.hpp"
#include "util/enum.hpp"
#include "util/exceptions.hpp"
#include "util/json/array.hpp"
#include "util/json/object.hpp"
#include "util/json/value.hpp"
#include <nlohmann/json.hpp>

namespace VTX::Util::JSon
{
	class Document::Impl
	{
	  public:
		Impl() {}
		Impl( const Impl & p_source ) = default;

		Impl( const BasicJSon & p_init ) { _jsonDocument = { convert( p_init ) }; }

		Impl( const std::initializer_list<BasicJSon> && p_init ) : Impl( std::vector<BasicJSon>( p_init ) ) {}
		Impl( const std::vector<BasicJSon> & p_init )
		{
			if ( p_init.size() == 0 )
			{
				_jsonDocument = {};
			}
			else if ( p_init.size() == 1 )
			{
				_jsonDocument = { convert( *p_init.begin() ) };
			}
			else if ( _isObjectField( p_init ) )
			{
				const std::vector<BasicJSon> vecInit = p_init;
				const Object::Field			 field	 = { vecInit[ 0 ].getValue().getString(), vecInit[ 1 ] };

				_jsonDocument = convert( Object( { field } ) );
			}
			else
			{
				_jsonDocument = convert( Array( p_init ) );
			}
		}

		void		clear() { _jsonDocument.clear(); }
		std::string toString() const { return _jsonDocument.dump(); }

	  private:
		nlohmann::json _jsonDocument = nlohmann::json();

		nlohmann::json::value_type convert( const BasicJSon & p_basicJSon )
		{
			switch ( p_basicJSon.getType() )
			{
			case BasicJSon::EnumType::Array: return convert( p_basicJSon.getArray() ); break;
			case BasicJSon::EnumType::BaseValue: return convert( p_basicJSon.getValue() ); break;
			case BasicJSon::EnumType::Object: return convert( p_basicJSon.getObject() ); break;
			case BasicJSon::EnumType::Document: return convert( p_basicJSon.getDocument() ); break;

			default:
				throw( VTXException(
					"Unmanaged type " + std::string( Enum::enumName( p_basicJSon.getType() ) ) + " in JSon parser."
				) );
			}
		}

		nlohmann::json convert( const Document & p_document ) { return p_document._impl->_jsonDocument; }

		nlohmann::json::array_t convert( const Array & p_array )
		{
			nlohmann::json::array_t res = nlohmann::json::array_t();

			for ( const BasicJSon & item : p_array )
			{
				res.emplace_back( convert( item ) );
			}

			return res;
		}

		nlohmann::json::object_t convert( const Object & p_object )
		{
			nlohmann::json::object_t res = nlohmann::json::object_t();

			for ( const Object::Field & item : p_object )
			{
				res[ item.first ] = convert( item.second );
			}

			return res;
		}
		nlohmann::json::value_type convert( const Value & p_value )
		{
			switch ( p_value.getType() )
			{
			case Value::EnumType::Bool: return nlohmann::json::value_type( p_value.getBool() );
			case Value::EnumType::Numeral: return nlohmann::json::value_type( p_value.getNumberInteger() );
			case Value::EnumType::Floating: return nlohmann::json::value_type( p_value.getNumberFloat() );
			case Value::EnumType::String: return nlohmann::json::value_type( p_value.getString() );

			default:
				throw( VTXException(
					"Unmanaged type " + std::string( Enum::enumName( p_value.getType() ) ) + " in JSon parser."
				) );
			}
		}

		bool _isObjectField( const std::vector<BasicJSon> & p_init ) const
		{
			if ( p_init.size() != 2 )
				return false;

			const BasicJSon & firstItem = *p_init.begin();

			return firstItem.getType() == BasicJSon::EnumType::BaseValue
				   && firstItem.getValue().getType() == Value::EnumType::String;
		}
	};

	Document::Document() : _impl( std::make_unique<Impl>() ) {}
	Document::Document( const Document & p_source ) : _impl( std::make_unique<Impl>( *p_source._impl ) ) {};

	Document::Document( const BasicJSon & p_init ) : _impl( std::make_unique<Impl>( p_init ) ) {};
	Document::Document( const std::initializer_list<BasicJSon> & p_init ) :
		_impl( std::make_unique<Impl>( std::vector<BasicJSon>( p_init ) ) ) {};

	Document::~Document() {}

	Document & Document::operator=( const Document & p_source )
	{
		_impl = std::make_unique<Impl>( *p_source._impl );
		return *this;
	}

	void Document::clear() { _impl->clear(); }

	std::string Document::toString() const { return _impl->toString(); }

} // namespace VTX::Util::JSon
