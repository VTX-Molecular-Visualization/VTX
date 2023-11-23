#ifndef __VTX_UTIL_JSON_DOCUMENT__
#define __VTX_UTIL_JSON_DOCUMENT__

#include "_fwd.hpp"
#include "basic_json.hpp"
#include "conversion.hpp"
#include <cstdarg>
#include <memory>
#include <string>
#include <vector>

namespace VTX::Util::JSon
{
	class Document
	{
	  public:
		Document();
		Document( const Document & p_source );

		Document( const BasicJSon & p_init );
		Document( const std::initializer_list<BasicJSon> & p_init );

		template<BasicJSonConcept T>
		Document( const T & p_value ) : Document( Conversion::toBasicJSon( p_value ) )
		{
		}

		// template<BasicJSonConcept... Args>
		// Document( std::tuple<Args...> p_values )
		//{
		//	std::vector<BasicJSon> vecValues = std::vector<BasicJSon>();
		//	vecValues.resize( sizeof...( p_values ) );

		//	for ( size_t i = 0; i < vecValues.size(); i++ )
		//	{
		//		vecValues[ i ] = Conversion::toBasicJSon( std::get<i>( vecValues ) );
		//	}

		//	_init( vecValues );
		//}

		// template<ObjectConcept T>
		// Document( const T & p_value ) : Document( BasicJSon( Object( p_value ) ) )
		//{
		// }
		// template<ArrayConcept T>
		// Document( const T & p_value ) : Document( BasicJSon( Array( p_value ) ) )
		//{
		// }
		// template<ValueConcept T>
		// Document( const T & p_value ) : Document( BasicJSon( Value( p_value ) ) )
		//{
		// }

		// Document( const Value & p_json );
		// Document( const Object & p_json );
		// Document( const Array & p_json );

		~Document();

		Document & operator=( const Document & p_source );

		void		clear();
		std::string toString() const;

	  private:
		class Impl;
		std::unique_ptr<Impl> _impl;

		void _init( const std::vector<BasicJSon> & p_initVector );
	};

} // namespace VTX::Util::JSon
#endif
