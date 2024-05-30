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
			SetBuffSize( 50 );
			ReserveModelFrame(modelFrameIdx);
		}
		Frame & WriteElement( const Frame & elem )
		{
			if ( GetWriteIdx() == modelFrameIdx)
				updateWriteIdx();
			return CircularBuffer<Frame>::WriteElement( elem );
		}
		bool ReadElement( Frame & elem )
		{
			if ( GetReadIdx() == modelFrameIdx)
				updateReadIdx();
			return CircularBuffer<Frame>::ReadElement(elem);
		}
		Frame & ReadElement( void )
		{
			if ( GetReadIdx() == modelFrameIdx )
				updateReadIdx();
			return CircularBuffer<Frame>::ReadElement();
		}
		void SetTotalElements( const size_t size ) { totalElements = size; }
		
		size_t GetTotalElements( void ) const { return totalElements; }

		void ReserveModelFrame( size_t frameIdx )
		{
			modelFrameIdx = frameIdx;
		}
		Frame & GetModelFrame( void ) { return GetElement( modelFrameIdx ); }
		const Frame & GetModelFrame( void ) const { return GetElement( modelFrameIdx ); }

	  private:
		size_t				 totalElements; // TODO change to double if possible
		size_t modelFrameIdx;
	};
} // namespace VTX::Core::Struct

#endif
