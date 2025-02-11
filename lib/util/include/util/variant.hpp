#ifndef __VTX_UTIL_VARIANT__
#define __VTX_UTIL_VARIANT__

#include <concepts>
#include <map>
#include <string>
#include <type_traits>
#include <util/concepts.hpp>
#include <util/types.hpp>
#include <variant>

namespace VTX::Util
{
	template<typename T>
	concept Vec3fConcept = std::same_as<Vec3f, T>;
	template<typename T>
	concept Vec4fConcept = std::same_as<Vec4f, T>;

	template<typename T>
	concept VariantPlainValue
		= ( std::integral<T> || std::floating_point<T> || StringConcept<T> || std::same_as<Vec3f, T>
			|| std::same_as<Vec4f, T> );

	template<typename T>
	concept VariantPtrValue = ( !VariantPlainValue<T> ) && std::is_pointer<T>::value;

	template<typename T>
	concept VariantValueConcept = ( VariantPlainValue<T> || VariantPtrValue<T> );

	namespace Variant
	{
		template<class... Args>
		struct StructCastProxy
		{
			std::variant<Args...> v;

			template<class... ToArgs>
			operator std::variant<ToArgs...>() const
			{
				return std::visit( []( auto && arg ) -> std::variant<ToArgs...> { return arg; }, v );
			}
		};

		template<class... Args>
		StructCastProxy<Args...> cast( const std::variant<Args...> & p_v )
		{
			return { p_v };
		}
	} // namespace Variant

	// Default VTX variant which contains VTX base types
	class VTXVariant
	{
	  private:
		using variant_t = std::variant<size_t, double, std::string, Vec3f, Vec4f, void *>;

	  public:
		VTXVariant() : _variant() {}
		VTXVariant( const VTXVariant & p_source ) = default;

		template<std::integral T>
		VTXVariant( const T & p_value ) : _variant( size_t( p_value ) )
		{
		}
		template<std::floating_point T>
		VTXVariant( const T & p_value ) : _variant( double( p_value ) )
		{
		}
		template<StringConcept T>
		VTXVariant( const T & p_value ) : _variant( std::string( p_value ) )
		{
		}
		VTXVariant( const Vec3f & p_value ) : _variant( p_value ) {}
		VTXVariant( const Vec4f & p_value ) : _variant( p_value ) {}

		template<VariantPtrValue T>
		VTXVariant( const T & p_ptr ) : _variant( static_cast<void * const>( p_ptr ) )
		{
		}

		template<std::integral T>
		bool is() const
		{
			return std::holds_alternative<size_t>( _variant );
		}
		template<std::floating_point T>
		bool is() const
		{
			return std::holds_alternative<double>( _variant );
		}
		template<StringConcept T>
		bool is() const
		{
			return std::holds_alternative<std::string>( _variant );
		}
		template<Vec3fConcept T>
		bool is() const
		{
			return std::holds_alternative<Vec3f>( _variant );
		}
		template<Vec4fConcept T>
		bool is() const
		{
			return std::holds_alternative<Vec4f>( _variant );
		}

		bool isPtr() const { return std::holds_alternative<void *>( _variant ); }

		template<std::integral T>
		T get() const
		{
			return T( std::get<size_t>( _variant ) );
		}
		template<std::floating_point T>
		T get() const
		{
			return T( std::get<double>( _variant ) );
		}
		template<StringConcept T>
		T get() const
		{
			return T( std::get<std::string>( _variant ) );
		}
		template<Vec3fConcept T>
		T get() const
		{
			return T( std::get<Vec3f>( _variant ) );
		}
		template<Vec4fConcept T>
		T get() const
		{
			return T( std::get<Vec4f>( _variant ) );
		}

		template<typename T>
		T * getPtr() const
		{
			return static_cast<T *>( std::get<void *>( _variant ) );
		}

	  private:
		variant_t _variant;
	};

	using VariantMap = std::map<std::string, Util::VTXVariant>;

} // namespace VTX::Util
#endif
