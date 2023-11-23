#ifndef __VTX_UTIL_JSON_VALUE__
#define __VTX_UTIL_JSON_VALUE__

#include "_fwd.hpp"
#include <string>
#include <variant>

namespace VTX::Util::JSon
{
	class Value
	{
	  public:
		enum class EnumType : int
		{
			Bool,
			Numeral,
			Floating,
			String,
		};

		Value( const bool p_value );
		Value( const int p_value );
		Value( const size_t p_value );
		Value( const float p_value );
		Value( const std::string & p_value );
		Value( const char * p_value );

		EnumType getType() const { return _type; }

		bool				getBool() const;
		size_t				getNumberInteger() const;
		float				getNumberFloat() const;
		const std::string & getString() const;

	  private:
		std::variant<bool, size_t, float, std::string> _value = std::variant<bool, size_t, float, std::string>();
		EnumType									   _type;
	};

	template<typename T>
	concept ValueCompatibleConcept = requires( T obj ) { new Util::JSon::Value( obj ); };

	template<typename T>
	concept ValueConcept = std::convertible_to<T, Util::JSon::Value> || ValueCompatibleConcept<T>;
} // namespace VTX::Util::JSon
#endif
