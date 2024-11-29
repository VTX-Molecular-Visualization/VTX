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
			setBuffSize( 102 );
		}
		Frame & writeElement( const Frame & elem )
		{
			return CircularBuffer<Frame>::writeElement( elem );
		}
		bool readElement( Frame & elem )
		{
			return CircularBuffer<Frame>::readElement(elem);
		}
		Frame & readElement( void )
		{
			return CircularBuffer<Frame>::readElement();
		}
		void setTotalElements( const size_t size ) { totalElements = size; }
		
		size_t getTotalElements( void ) const { return totalElements; }

		Frame & GetModelFrame( void ) { return getElement( 0 ); }
		const Frame & GetModelFrame( void ) const { return getElement( 0 ); }

	  private:
		size_t				 totalElements; // TODO change to double if possible
	};
} // namespace VTX::Core::Struct

#endif
