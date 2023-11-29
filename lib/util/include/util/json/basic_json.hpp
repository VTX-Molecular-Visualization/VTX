#ifndef __VTX_UTIL_JSON_BASIC_JSON__
#define __VTX_UTIL_JSON_BASIC_JSON__

#include "_fwd.hpp"
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace VTX::Util::JSon
{
	class BasicJSon
	{
	  private:
		using VariantType = std::variant<
			std::monostate,
			bool,
			size_t,
			double,
			std::string,
			std::unique_ptr<Array>,
			std::unique_ptr<Object>,
			std::unique_ptr<Document>>;

	  public:
		enum class EnumType : int
		{
			Bool,
			Integral,
			FloatingPoint,
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
		BasicJSon( const double p_value );
		BasicJSon( const std::string & p_value );
		BasicJSon( const char * p_value );
		BasicJSon( const Array & p_value );
		BasicJSon( const Object & p_value );
		BasicJSon( const Document & p_value );
		BasicJSon( std::unique_ptr<Array> p_value );
		BasicJSon( std::unique_ptr<Object> p_value );
		BasicJSon( std::unique_ptr<Document> p_value );

		~BasicJSon();

		BasicJSon & operator=( const BasicJSon & p_source );

		EnumType getType() const { return _type; }

		bool				getBool() const;
		size_t				getIntegral() const;
		double				getFloatingPoint() const;
		const std::string & getString() const;
		const Array &		getArray() const;
		const Object &		getObject() const;
		const Document &	getDocument() const;

		Array &	 getArrayEditable() const;
		Object & getObjectEditable() const;

		operator const Array &() const;
		operator Array &() const;
		operator const Object &() const;
		operator Object &() const;

		template<typename T>
		const T & get() const
		{
			return std::get<T>( _value );
		}
		template<std::integral T>
		const T get() const
		{
			return std::get<size_t>( _value );
		}
		template<std::floating_point T>
		const T get() const
		{
			return std::get<double>( _value );
		}

		const BasicJSon & operator[]( const std::string & p_key ) const;
		BasicJSon &		  operator[]( const std::string & p_key );

		const BasicJSon & operator[]( const size_t & p_index ) const;
		BasicJSon &		  operator[]( const size_t & p_index );

		bool contains( const std::string & p_key ) const;

	  private:
		EnumType	_type = EnumType::Unknown;
		VariantType _value;

		static bool _isDescribingObject( const std::initializer_list<BasicJSon> & p_data );
		static bool _isObjectField( const BasicJSon & p_potentialField );
		static void _fillObjectFieldsFromVector( Object & p_obj, const std::initializer_list<BasicJSon> & p_vector );

		void _copyFrom( const BasicJSon & p_source );
	};

} // namespace VTX::Util::JSon
#endif
