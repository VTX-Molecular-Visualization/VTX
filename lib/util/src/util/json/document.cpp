#include "util/json/document.hpp"
#include "util/enum.hpp"
#include "util/exceptions.hpp"
#include "util/json/array.hpp"
#include "util/json/object.hpp"
#include <nlohmann/json.hpp>

namespace VTX::Util::JSon
{
	class Document::Impl
	{
	  public:
		Impl() {}
		Impl( const Impl & p_source ) = default;

		Impl( const BasicJSon & p_init ) { _jsonDocument = { convert( p_init ) }; }

		Impl( const std::initializer_list<BasicJSon> & p_init )
		{
			if ( p_init.size() == 0 )
			{
				_jsonDocument = {};
			}
			else if ( _isDescribingObject( p_init ) )
			{
				Object obj	  = _createObjectFromVector( p_init );
				_jsonDocument = convert( obj );
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
			case BasicJSon::EnumType::Bool: return nlohmann::json::value_type( p_basicJSon.getBool() );
			case BasicJSon::EnumType::Numeral: return nlohmann::json::value_type( p_basicJSon.getNumberInteger() );
			case BasicJSon::EnumType::Floating: return nlohmann::json::value_type( p_basicJSon.getNumberFloat() );
			case BasicJSon::EnumType::String: return nlohmann::json::value_type( p_basicJSon.getString() );
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

		bool _isDescribingObject( const std::initializer_list<BasicJSon> & p_data )
		{
			for ( const BasicJSon & potentialField : p_data )
			{
				if ( !_isObjectField( potentialField ) )
					return false;
			}

			return true;
		}

		bool _isObjectField( const BasicJSon & p_potentialField ) const
		{
			if ( p_potentialField.getType() != BasicJSon::EnumType::Array )
				return false;

			const Array & potentialFieldArray = p_potentialField.getArray();

			return potentialFieldArray.size() == 2
				   && potentialFieldArray.begin()->getType() == BasicJSon::EnumType::String;
		}

		Object _createObjectFromVector( const std::initializer_list<BasicJSon> & p_vector ) const
		{
			Object res = Object();

			for ( const BasicJSon & json : p_vector )
			{
				const Array & arrayField = json.getArray();
				res.appendField( arrayField[ 0 ].getString(), arrayField[ 1 ] );
			}

			return res;
		}
	};

	Document::Document() : _impl( std::make_unique<Impl>() ) {}
	Document::Document( const Document & p_source ) : _impl( std::make_unique<Impl>( *p_source._impl ) ) {};

	Document::Document( const BasicJSon & p_init ) : _impl( std::make_unique<Impl>( p_init ) ) {};
	Document::Document( const std::initializer_list<BasicJSon> & p_init ) :
		_impl( std::make_unique<Impl>( p_init ) ) {};

	Document::~Document() {}

	Document & Document::operator=( const Document & p_source )
	{
		_impl = std::make_unique<Impl>( *p_source._impl );
		return *this;
	}

	void Document::clear() { _impl->clear(); }

	std::string Document::toString() const { return _impl->toString(); }

} // namespace VTX::Util::JSon
