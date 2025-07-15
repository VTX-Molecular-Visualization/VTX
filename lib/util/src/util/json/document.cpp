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

		Impl( const BasicJSon & p_init ) { _jsonDocument = convert( p_init ); }

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
		std::string getContentAsString() const { return _jsonDocument.dump(); }

		void setContent( const nlohmann::json & p_document ) { _jsonDocument = p_document; }

		void generate( Document & p_document ) { p_document._json = create( _jsonDocument ); }

		BasicJSon create( const nlohmann::json & p_json ) const
		{
			BasicJSon child;

			switch ( p_json.type() )
			{
			case nlohmann::json::value_t::boolean: return BasicJSon( p_json.get<bool>() ); break;
			case nlohmann::json::value_t::number_float: return BasicJSon( p_json.get<float>() ); break;
			case nlohmann::json::value_t::number_unsigned: return BasicJSon( p_json.get<size_t>() ); break;
			case nlohmann::json::value_t::string: return BasicJSon( p_json.get<std::string>() ); break;
			case nlohmann::json::value_t::array: child = BasicJSon( std::move( createArray( p_json ) ) ); break;
			case nlohmann::json::value_t::object:
				child = BasicJSon( createObject( nlohmann::json::object_t( p_json ) ) );
				break;

			case nlohmann::json::value_t::discarded:
			case nlohmann::json::value_t::null:
			case nlohmann::json::value_t::binary:
			default:
				VTX_ERROR( "Unmanaged json type {}", Enum::enumName( p_json.type() ) );
				child = BasicJSon();
				break;
			}

			return child;
		}

		std::unique_ptr<Array> createArray( const nlohmann::json & p_jsonArray ) const
		{
			std::unique_ptr<Array> res = std::make_unique<Array>();

			for ( const nlohmann::json & child : p_jsonArray )
			{
				res->emplace_back( create( child ) );
			}

			return res;
		}
		std::unique_ptr<Object> createObject( const nlohmann::json::object_t & p_jsonObject ) const
		{
			std::unique_ptr<Object> res = std::make_unique<Object>();

			for ( auto & field : p_jsonObject )
			{
				res->appendField( field.first, create( field.second ) );
			}

			return res;
		}

	  private:
		nlohmann::json _jsonDocument = nlohmann::json();

		nlohmann::json::value_type convert( const BasicJSon & p_basicJSon )
		{
			switch ( p_basicJSon.getType() )
			{
			case BasicJSon::EnumType::Array: return convert( p_basicJSon.getArray() ); break;
			case BasicJSon::EnumType::Bool: return nlohmann::json::value_type( p_basicJSon.getBool() );
			case BasicJSon::EnumType::Integral: return nlohmann::json::value_type( p_basicJSon.getIntegral() );
			case BasicJSon::EnumType::FloatingPoint:
				return nlohmann::json::value_type( p_basicJSon.getFloatingPoint() );
			case BasicJSon::EnumType::String: return nlohmann::json::value_type( p_basicJSon.getString() );
			case BasicJSon::EnumType::Object: return convert( p_basicJSon.getObject() ); break;
			case BasicJSon::EnumType::Document: return convert( p_basicJSon.getDocument() ); break;

			default:
				throw( VTXException(
					"Unmanaged type " + std::string( Enum::enumName( p_basicJSon.getType() ) ) + " in JSon parser."
				) );
			}
		}

		nlohmann::json			convert( const Document & p_document ) { return p_document._impl->_jsonDocument; }
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

	Document Document::createFromString( const std::string & p_content )
	{
		const nlohmann::json json = nlohmann::json::parse( p_content );

		Document res = Document();
		res._impl->setContent( json );
		res._generate();

		return res;
	}

	Document::Document() : _impl( std::make_unique<Impl>() ) {}
	Document::Document( const Document & p_source ) : _impl( std::make_unique<Impl>( *p_source._impl ) )
	{
		_generate();
	};

	Document::Document( const BasicJSon & p_init ) : _impl( std::make_unique<Impl>( p_init ) ) { _generate(); };
	Document::Document( const std::initializer_list<BasicJSon> & p_init ) : _impl( std::make_unique<Impl>( p_init ) )
	{
		_generate();
	};

	Document::~Document() {}

	Document & Document::operator=( const Document & p_source )
	{
		_impl = std::make_unique<Impl>( *p_source._impl );
		_json = p_source._json;
		return *this;
	}

	const BasicJSon & Document::json() const { return _json; }
	BasicJSon &		  Document::json() { return _json; }

	void Document::clear() { _impl->clear(); }

	std::string Document::getContentAsString() const { return _impl->getContentAsString(); }

	void Document::_generate() { _impl->generate( *this ); }

} // namespace VTX::Util::JSon
