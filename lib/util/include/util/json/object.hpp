#ifndef __VTX_UTIL_JSON_OBJECT__
#define __VTX_UTIL_JSON_OBJECT__

#include "util/json/basic_json.hpp"
#include <map>
#include <string>

namespace VTX::Util::JSon
{
	class Object
	{
	  public:
		using Field = std::pair<const std::string, BasicJSon>;

	  public:
		Object();
		Object( const Object & p_source ) = default;
		Object( const std::initializer_list<Field> & p_init );

		void appendField( const Field & p_field );
		void appendField( const std::string & p_key, const BasicJSon & p_value );

		std::map<std::string, BasicJSon>::const_iterator begin() const;
		std::map<std::string, BasicJSon>::const_iterator end() const;

		BasicJSon &		  operator[]( const std::string & p_key );
		const BasicJSon & operator[]( const std::string & p_key ) const;

	  private:
		std::map<std::string, BasicJSon> _map = std::map<std::string, BasicJSon>();
	};
} // namespace VTX::Util::JSon
#endif
