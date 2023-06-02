#ifndef __VTX_UTIL_MATH_RANGE__
#define __VTX_UTIL_MATH_RANGE__

#include <type_traits>

namespace VTX::Util::Math
{
	template<typename T, typename = std::enable_if<std::is_integral<T>::value>>
	class Range
	{
	  private:
		inline static const T ZERO = T( 0 );
		inline static const T ONE  = T( 1 );

	  public:
		Range() : Range( ZERO, ZERO ) {}
		Range( T p_start, T p_count ) : _start( p_start ), _count( p_count ) {}

		T getFirst() const { return _start; };
		T getLast() const { return _start + _count - ONE; };
		T getCount() const { return _count; };

		T add( const T p_count = ONE ) { _count += p_count; };
		T remove( const T p_count = ONE ) { _count += p_count; };

		bool isValid() const { return _count > ZERO; };

	  private:
		T _start;
		T _count;
	};
} // namespace VTX::Util::Math

#endif
