#ifndef __VTX_CORE_STRUCT_CIRCBUFF_PRODCONS__
#define __VTX_CORE_STRUCT_CIRCBUFF_PRODCONS__

#include <vector>
#include <util/logger.hpp>

namespace VTX::Core::Struct
{
	constexpr char tmp_circbuffmask_read_overflow { 0x1 };
	constexpr char tmp_circbuffmask_write_overflow { 0x2 };

	template<typename T>
	class ProdConsCircularBuffer
	{
	  public:
		ProdConsCircularBuffer() : readIdx( 0 ), writeIdx( 0 ), buffSize( 0 ), overflowFlag( 0 )
		{
			SetBuffSize( 0 );
		}
		// Currently this ctor is not used
		// Call UpdateBuffSize method to define buffer size
		ProdConsCircularBuffer( const size_t size ) :
			readIdx( 0 ), writeIdx( 0 ), buffSize( size ), overflowFlag( 0 )
		{
			SetBuffSize( size );
			return;
		}
		// FIXME really?
		ProdConsCircularBuffer( ProdConsCircularBuffer && movable ) // : circBuff( std::move( movable.circBuff ) )
		// mutex is default initialized?
		{
			circBuff	 = std::move( movable.circBuff );
			readIdx		 = movable.readIdx;
			writeIdx	 = movable.writeIdx;
			buffSize	 = movable.buffSize;
			overflowFlag = movable.overflowFlag;
		}
		ProdConsCircularBuffer & operator=( const ProdConsCircularBuffer && movable ) noexcept
		{
			circBuff = std::move( movable.circBuff );
			readIdx	 = movable.readIdx;
			writeIdx = movable.writeIdx;
			buffSize = movable.buffSize;
			overflowFlag = movable.overflowFlag;
			return *this;
		}
		T& WriteElement( const T &elem )
		{
			std::unique_lock<std::mutex> unique_lock( access_mtx );
			write_allowed.wait( unique_lock, [ this ]() { return isWriteAllowed(); });

			size_t currentWriteIdx = writeIdx;
			circBuff[ currentWriteIdx ] = std::move(elem);
			VTX_INFO( "WriteElement currentWriteIdx {}", currentWriteIdx );
			updateWriteIdx();

			unique_lock.unlock();
			read_allowed.notify_one();

			return circBuff[ currentWriteIdx ];
		}
		bool ReadElement( T& elem )
		{
			std::unique_lock<std::mutex> unique_lock( access_mtx );
			read_allowed.wait( unique_lock, [ this ]() { return isReadAllowed(); } );

			elem = circBuff[ readIdx ];
			VTX_INFO( "ReadElement readIdx {}", readIdx );
			updateReadIdx();

			unique_lock.unlock();
			write_allowed.notify_one();

			return true;
		}
		// FIXME where is it used?
		T & ReadElement( void )
		{
			std::unique_lock<std::mutex> unique_lock( access_mtx );
			read_allowed.wait( unique_lock, [ this ]() { return isReadAllowed(); } );

			updateReadIdx();

			unique_lock.unlock();
			write_allowed.notify_one();

			return circBuff[ readIdx ];
		}
		void EraseEmptyFrames(void)
		{
			/* do
			{
				trajectory.frames.pop_back();
			} while ( trajectory.frames.back().size() == 0 ); */ 
			// TODO
			return;
		}
		size_t GetBuffSize(void) const { return buffSize; }

		void Reset(void)
		{
			// FIXME reset also the content of the buffer?
			readIdx = 0;
			writeIdx = 0;
		}

	  protected:
		// FIXME
		const T & GetElement( size_t index ) const { return circBuff[ index%buffSize ]; }
		T & GetElement( size_t index ) { return circBuff[ index % buffSize ]; }

		void updateWriteIdx( void )
		{
			writeIdx >= buffSize - 1 ? writeIdx = 0 : writeIdx++;
			if ( !writeIdx )
				overflowFlag ^= tmp_circbuffmask_write_overflow;
		}
		void updateReadIdx( void )
		{
			readIdx >= buffSize - 1 ? readIdx = 0 : readIdx++;
			if ( !readIdx )
				overflowFlag ^= tmp_circbuffmask_read_overflow;
		}
		void SetBuffSize(size_t size)
		{
			circBuff.resize( size );
			buffSize = size;
		}
		size_t GetReadIdx( void ) const { return readIdx; }
		size_t GetWriteIdx( void ) const { return writeIdx; }
	  private:
		size_t				 buffSize;
		size_t				 readIdx;
		size_t				 writeIdx;
		std::vector<T>		 circBuff;
		char				 overflowFlag;
		// concurency
		std::mutex access_mtx;
		std::condition_variable write_allowed;
		std::condition_variable read_allowed;

		bool isReadAllowed( void ) const
		{
			bool allowed( false );

			if ( 0 == buffSize )
				return allowed;

			if ((bool)(overflowFlag & tmp_circbuffmask_read_overflow) == (bool)(overflowFlag & tmp_circbuffmask_write_overflow))
				allowed = readIdx < writeIdx;
			else
				allowed = readIdx >= writeIdx;

			return allowed;
		}
		
		bool isWriteAllowed( void ) const 
		{ 
			//return !isReadAllowed();
			return true;
		}
	};
} // namespace VTX::Core::Struct

#endif
