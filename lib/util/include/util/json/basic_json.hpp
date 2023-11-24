#ifndef __VTX_UTIL_JSON_BASIC_JSON__
#define __VTX_UTIL_JSON_BASIC_JSON__

#include "_fwd.hpp"
#include "value.hpp"
#include <string>
#include <variant>
#include <vector>

namespace VTX::Util::JSon
{
	class BasicJSon
	{
	  public:
		enum class EnumType : int
		{
			BaseValue,
			Array,
			Object,
			Document,
			Unknown,
		};

		BasicJSon();
		BasicJSon( const BasicJSon & p_source );

		BasicJSon( const Array & p_value );
		BasicJSon( const Value & p_value );
		BasicJSon( const Object & p_value );
		BasicJSon( const Document & p_value );

		BasicJSon( std::initializer_list<BasicJSon> p_init );

		template<ValueCompatibleConcept T>
		BasicJSon( const T & p_value ) : BasicJSon( Value( p_value ) )
		{
		}

		EnumType getType() const { return _type; }

		const Value &	 getValue() const;
		const Array &	 getArray() const;
		const Object &	 getObject() const;
		const Document & getDocument() const;

		void setValue( const Array & p_value );
		void setValue( const Value & p_value );
		void setValue( const Object & p_value );
		void setValue( const Document & p_value );

	  private:
		EnumType															 _type = EnumType::Unknown;
		std::variant<std::monostate, Value *, Array *, Object *, Document *> _value
			= std::variant<std::monostate, Value *, Array *, Object *, Document *>();

		bool _isObjectField( const std::vector<BasicJSon> & p_init ) const;
	};
} // namespace VTX::Util::JSon
#endif
