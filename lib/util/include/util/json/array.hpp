#ifndef __VTX_UTIL_JSON_ARRAY__
#define __VTX_UTIL_JSON_ARRAY__

#include "util/json/basic_json.hpp"
#include <vector>

namespace VTX::Util::JSon
{
	class Array
	{
	  public:
		Array();
		Array( const Array & p_source );

		Array( const std::initializer_list<BasicJSon> & p_init );
		Array( const std::vector<BasicJSon> & p_init );

		~Array();

		BasicJSon &		  operator[]( const size_t p_index );
		const BasicJSon & operator[]( const size_t p_index ) const;

		std::vector<BasicJSon>::const_iterator begin() const;
		std::vector<BasicJSon>::const_iterator end() const;

		void emplace_back( const BasicJSon & p_obj );

		size_t size() const;

	  private:
		std::vector<BasicJSon> _objs = std::vector<BasicJSon>();
	};
} // namespace VTX::Util::JSon
#endif
