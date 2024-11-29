#ifndef __VTX_CORE_STRUCT_CIRCBUFF__
#define __VTX_CORE_STRUCT_CIRCBUFF__

#include <vector>
#include <util/logger.hpp>

namespace VTX::Core::Struct
{
	constexpr char circbuffmask_read_overflow { 0x1 };
	constexpr char circbuffmask_write_overflow { 0x2 };

	template<typename T>
	class CircularBuffer
	{
	  public:
		CircularBuffer() : readIdx( 0 ), writeIdx( 0 ), buffSize( 0 ), overflowFlag( 0 )
		{
			setBuffSize( 0 );
		}
		// Currently this ctor is not used
		// Call UpdateBuffSize method to define buffer size
		CircularBuffer( const size_t size ) :
			readIdx( 0 ), writeIdx( 0 ), buffSize( size ), overflowFlag( 0 )
		{
			setBuffSize( size );
			return;
		}
		T& writeElement( const T &elem )
		{
			size_t currentWriteIdx = writeIdx;
			circBuff[ currentWriteIdx ] = elem;
			updateWriteIdx();
			return circBuff[ currentWriteIdx ];
		}
		bool readElement( T& elem )
		{
			if ( !isReadAllowed() )
				return false;
			elem = circBuff[ readIdx ];
			VTX_INFO( "ReadElement readIdx {}", readIdx );
			updateReadIdx();
			return true;
		}
		T & readElement( void )
		{
			if ( !isReadAllowed() )
				return circBuff[ 0 ]; // FIXME NOPE
			updateReadIdx();
			return circBuff[ readIdx ];
		}
		void eraseEmptyFrames(void)
		{
			/* do
			{
				trajectory.frames.pop_back();
			} while ( trajectory.frames.back().size() == 0 ); */ 
			// TODO
			return;
		}
		size_t getBuffSize(void) const { return buffSize; }

	  protected:
		// FIXME
		const T & getElement( size_t index ) const { return circBuff[ index%buffSize ]; }
		T & getElement( size_t index ) { return circBuff[ index % buffSize ]; }

		void updateWriteIdx( void )
		{
			writeIdx >= buffSize - 1 ? writeIdx = 0 : writeIdx++;
			if ( !writeIdx )
				overflowFlag ^= circbuffmask_write_overflow;
		}
		void updateReadIdx( void )
		{
			readIdx >= buffSize - 1 ? readIdx = 0 : readIdx++;
			if ( !readIdx )
				overflowFlag ^= circbuffmask_read_overflow;
		}
		void setBuffSize(size_t size)
		{
			circBuff.resize( size );
			buffSize = size;
		}
		size_t getReadIdx( void ) const { return readIdx; }
		size_t getWriteIdx( void ) const { return writeIdx; }
	  private:
		size_t				 buffSize;
		size_t				 readIdx;
		size_t				 writeIdx;
		std::vector<T>		 circBuff;
		char				 overflowFlag;

		bool isReadAllowed( void ) const
		{
			bool allowed( false );

			if ( 0 == buffSize )
				return allowed;

			if ((bool)(overflowFlag & circbuffmask_read_overflow) == (bool)(overflowFlag & circbuffmask_write_overflow))
				allowed = readIdx < writeIdx;
			else
				allowed = readIdx >= writeIdx;

			return allowed;
		}
		
	};
} // namespace VTX::Core::Struct

#endif
