#ifndef __VTX_CORE_STRUCT_FRAMEBUFFSIMPLE__
#define __VTX_CORE_STRUCT_FRAMEBUFFSIMPLE__

#include "circular_buffer.hpp"
#include <util/types.hpp>

namespace VTX::Core::Struct
{
	using Frame = std::vector<Vec3f>;

	class FrameDataSimple : public CircularBuffer<Frame>
	{
	  public:
		FrameDataSimple() : totalElements( 0 )
		{
			SetBuffSize( 102 );
		}
		Frame & WriteElement( const Frame & elem )
		{
			return CircularBuffer<Frame>::WriteElement( elem );
		}
		bool ReadElement( Frame & elem )
		{
			return CircularBuffer<Frame>::ReadElement(elem);
		}
		Frame & ReadElement( void )
		{
			return CircularBuffer<Frame>::ReadElement();
		}
		void SetTotalElements( const size_t size ) { totalElements = size; }
		
		size_t GetTotalElements( void ) const { return totalElements; }

		Frame & GetModelFrame( void ) { return GetElement( 0 ); }
		const Frame & GetModelFrame( void ) const { return GetElement( 0 ); }

	  private:
		size_t				 totalElements; // TODO change to double if possible
	};
} // namespace VTX::Core::Struct

#endif
