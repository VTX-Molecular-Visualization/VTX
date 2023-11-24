#ifndef __VTX_UTIL_JSON_ARRAY__
#define __VTX_UTIL_JSON_ARRAY__

#include "util/json/basic_json.hpp"
#include "util/json/conversion.hpp"
#include <vector>

namespace VTX::Util::JSon
{
	class Array
	{
	  public:
		Array();
		Array( const Array & p_source );

		Array( const BasicJSon & p_singleValue );

		// template<BasicJSonConcept T>
		// Array( const T & p_singleValue ) : Array( Conversion::toBasicJSon( p_singleValue ) )
		//{
		// }

		Array( const std::initializer_list<BasicJSon> & p_init );
		Array( const std::vector<BasicJSon> & p_init );

		// template<BasicJSonConcept T>
		// Array( const std::initializer_list<T> & p_init )
		//{
		//	_objs.reserve( p_init.size() );

		//	for ( const T & obj : p_init )
		//	{
		//		_objs.emplace_back( Conversion::toBasicJSon( obj ) );
		//	}
		//}

		~Array();

		std::vector<BasicJSon>::const_iterator begin() const;
		std::vector<BasicJSon>::const_iterator end() const;

		void emplace_back( const BasicJSon & p_obj );

	  private:
		std::vector<BasicJSon> _objs = std::vector<BasicJSon>();
	};
} // namespace VTX::Util::JSon
#endif
