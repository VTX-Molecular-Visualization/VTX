#ifndef __VTX_UTIL_JSON_BASIC_JSON__
#define __VTX_UTIL_JSON_BASIC_JSON__

#include "_fwd.hpp"
#include <string>
#include <variant>
#include <vector>

namespace VTX::Util::JSon
{
	class BasicJSon
	{
	  private:
		using VariantType
			= std::variant<std::monostate, bool, size_t, float, std::string, Array *, Object *, Document *>;

	  public:
		enum class EnumType : int
		{
			Bool,
			Numeral,
			Floating,
			String,
			Array,
			Object,
			Document,
			Unknown,
		};

		BasicJSon();
		BasicJSon( const BasicJSon & p_source );
		BasicJSon( std::initializer_list<BasicJSon> p_init );

		BasicJSon( const bool p_value );
		BasicJSon( const int p_value );
		BasicJSon( const size_t p_value );
		BasicJSon( const float p_value );
		BasicJSon( const std::string & p_value );
		BasicJSon( const char * p_value );
		BasicJSon( const Array & p_value );
		BasicJSon( const Object & p_value );
		BasicJSon( const Document & p_value );

		~BasicJSon();

		EnumType getType() const { return _type; }

		bool				getBool() const;
		size_t				getNumberInteger() const;
		float				getNumberFloat() const;
		const std::string & getString() const;
		const Array &		getArray() const;
		const Object &		getObject() const;
		const Document &	getDocument() const;

	  private:
		EnumType	_type  = EnumType::Unknown;
		VariantType _value = VariantType();

		static bool _isDescribingObject( const std::initializer_list<BasicJSon> & p_data );
		static bool _isObjectField( const BasicJSon & p_potentialField );
		static void _fillObjectFieldsFromVector( Object & p_obj, const std::initializer_list<BasicJSon> & p_vector );
	};

	template<typename T>
	concept BasicJSonConcept = requires( T obj ) { BasicJSon( obj ); };
} // namespace VTX::Util::JSon
#endif
