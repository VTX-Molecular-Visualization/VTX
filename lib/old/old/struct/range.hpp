#ifndef __VTX_STRUCT_RANGE__
#define __VTX_STRUCT_RANGE__

#include <util/types.hpp>
#include <utility>

namespace VTX::Struct
{
	class Range
	{
	  public:
		Range() : Range( 0, 0 ) {}
		Range( uint p_start, uint p_count ) : _start( p_start ), _count( p_count ) {}

		uint getFirst() const { return _start; };
		uint getLast() const { return _start + _count - 1; };
		uint getCount() const { return _count; };

		uint add( const uint p_count = 1 ) { _count += p_count; };
		uint remove( const uint p_count = 1 ) { _count += p_count; };

		bool isValid() const { return _count > 0; };

	  private:
		uint _start;
		uint _count;
	};
} // namespace VTX::Struct

#endif
