#ifndef __VTX_UTIL_JSON_OBJECT__
#define __VTX_UTIL_JSON_OBJECT__

#include "_fwd.hpp"
#include "util/json/basic_json.hpp"
#include <map>
#include <string>

namespace VTX::Util::JSon
{
	class Object
	{
	  public:
		using Field = std::pair<const std::string, const BasicJSon>;

	  public:
		Object();
		Object( const std::string p_key, const BasicJSon & p_value );
		Object( const Field & p_field );
		Object( const std::initializer_list<Field> & p_init );

		void appendField( const Field & p_field );

		std::map<std::string, BasicJSon>::const_iterator begin() const;
		std::map<std::string, BasicJSon>::const_iterator end() const;

	  private:
		std::map<std::string, BasicJSon> _map = std::map<std::string, BasicJSon>();
	};
} // namespace VTX::Util::JSon
#endif
