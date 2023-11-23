#ifndef __VTX_UTIL_JSON_ARRAY__
#define __VTX_UTIL_JSON_ARRAY__

#include "util/json/basic_json.hpp"
#include "util/json/conversion.hpp"
#include <tuple>
#include <vector>

namespace VTX::Util::JSon
{
	class Array
	{
	  public:
		Array();
		Array( const Array & p_source );
		Array( const BasicJSon & p_singleValue );
		Array( const std::initializer_list<BasicJSon> & p_init );
		Array( const std::vector<BasicJSon> & p_init );

		template<BasicJSonConcept... Args>
		Array( std::tuple<Args...> p_values )
		{
			size_t argSize = sizeof...( p_values );
			_objs.resize( argSize );

			for ( size_t i = 0; i < argSize; i++ )
			{
				_objs[ i ] = Conversion::toBasicJSon( std::get<i>( p_values ) );
			}
		}

		~Array();

		std::vector<BasicJSon>::const_iterator begin() const;
		std::vector<BasicJSon>::const_iterator end() const;

		void emplace_back( const BasicJSon & p_obj );

	  private:
		std::vector<BasicJSon> _objs = std::vector<BasicJSon>();
	};
} // namespace VTX::Util::JSon
#endif
