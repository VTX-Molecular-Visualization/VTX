#ifndef __VTX_CORE_STRUCT_FRAMEBUFF__
#define __VTX_CORE_STRUCT_FRAMEBUFF__

#include "circular_buffer.hpp"
#include <util/types.hpp>

namespace VTX::Core::Struct
{
	using Frame = std::vector<Vec3f>;

	class FrameData : public CircularBuffer<Frame>
	{
	  public:
		FrameData() : totalElements( 0 ), modelFrameIdx( 0 )
		{
			setBuffSize( 50 );
			ReserveModelFrame(modelFrameIdx);
		}
		Frame & writeElement( const Frame & elem )
		{
			if ( getWriteIdx() == modelFrameIdx)
				updateWriteIdx();
			return CircularBuffer<Frame>::writeElement( elem );
		}
		bool readElement( Frame & elem )
		{
			if ( getReadIdx() == modelFrameIdx)
				updateReadIdx();
			return CircularBuffer<Frame>::readElement(elem);
		}
		Frame & readElement( void )
		{
			if ( getReadIdx() == modelFrameIdx )
				updateReadIdx();
			return CircularBuffer<Frame>::readElement();
		}
		void setTotalElements( const size_t size ) { totalElements = size; }
		
		size_t getTotalElements( void ) const { return totalElements; }

		void ReserveModelFrame( size_t frameIdx )
		{
			modelFrameIdx = frameIdx;
		}
		Frame & GetModelFrame( void ) { return getElement( modelFrameIdx ); }
		const Frame & GetModelFrame( void ) const { return getElement( modelFrameIdx ); }

	  private:
		size_t				 totalElements; // TODO change to double if possible
		size_t modelFrameIdx;
	};
} // namespace VTX::Core::Struct

#endif
