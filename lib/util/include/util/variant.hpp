#ifndef __VTX_UTIL_VARIANT__
#define __VTX_UTIL_VARIANT__

#include <map>
#include <string>
#include <util/types.hpp>
#include <variant>

namespace VTX
{
	class VTXVariant
	{
	  private:
		using variant_t = std::variant<int, uint, float, std::string, Vec3f, Vec4f, void *>;

	  public:
		VTXVariant() : _variant() {}

		template<typename T>
		VTXVariant( const T & p_value ) : _variant( p_value )
		{
		}
		template<typename T>
		VTXVariant( T * const p_ptr ) : _variant( static_cast<void * const>( p_ptr ) )
		{
		}

		template<typename T>
		T get() const
		{
			return std::get<T>( _variant );
		}

		template<typename T>
		T * getPtr() const
		{
			return static_cast<T *>( std::get<void *>( _variant ) );
		}

	  private:
		variant_t _variant;
	};

	using VariantMap = std::map<std::string, VTXVariant>;
} // namespace VTX
#endif