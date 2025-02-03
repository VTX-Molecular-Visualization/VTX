#ifndef __VTX_CORE_STRUCT_CIRCBUFF_PRODCONS__
#define __VTX_CORE_STRUCT_CIRCBUFF_PRODCONS__

#include <condition_variable>
#include <mutex>
#include <vector>

namespace VTX::Core::Struct
{
	constexpr char circbuffmask_read_overflow { 0x1 };
	constexpr char circbuffmask_write_overflow { 0x2 };

	/**
	 * @brief Circular buffer class storing any type of data.
	 * Producer Consumer pattern to handle multi threaded context.
	 * Keeps track of read and write operations with read and write indexes.
	 * Always allows write operations.
	 * Provides a blocking read system to ensure that the next read is either done on continuous data.
	 * If no continuity can be provided the equivalent of one buffer size of data will be skipped before read can
	 * continue. Done using overflow flags.
	 * There are two ways of accessing the data :
	 * Read operations to ensure a continuous read of the data holded and automatically updating the read index.
	 * Get operations to enable access to any element of the circular buffer at a given index and not updating the read
	 * index.
	 */
	template<typename T>
	class ProdConsCircularBuffer
	{
	  public:
		ProdConsCircularBuffer() : _readIdx( 0 ), _writeIdx( 0 ), _buffSize( 0 ), _overflowFlag( 0 )
		{
			setBuffSize( 0 );
		}
		/**
		 * @brief Currently this ctor is not used.
		 * Call UpdateBuffSize method to define buffer size.
		 */
		ProdConsCircularBuffer( const size_t size ) :
			_readIdx( 0 ), _writeIdx( 0 ), _buffSize( size ), _overflowFlag( 0 )
		{
			setBuffSize( size );
			return;
		}
		/**
		 * @brief Move ctor explicit because member section contains mutexes.
		 */
		ProdConsCircularBuffer( ProdConsCircularBuffer && movable )
		{
			_circBuff	  = std::move( movable._circBuff );
			_readIdx	  = movable._readIdx;
			_writeIdx	  = movable._writeIdx;
			_buffSize	  = movable._buffSize;
			_overflowFlag = movable._overflowFlag;
		}
		/**
		 * @brief Move assignement operator explicit because member section contains mutexes.
		 */
		ProdConsCircularBuffer & operator=( const ProdConsCircularBuffer && movable ) noexcept
		{
			_circBuff	  = std::move( movable._circBuff );
			_readIdx	  = movable._readIdx;
			_writeIdx	  = movable._writeIdx;
			_buffSize	  = movable._buffSize;
			_overflowFlag = movable._overflowFlag;
			return *this;
		}
		/**
		 * @brief Moves the content of the input element into the circular buffer at the current write index and
		 * increments write index.
		 */
		T & writeElement( const T & elem )
		{
			std::unique_lock<std::mutex> unique_lock( _access_mtx );
			_write_allowed.wait( unique_lock, [ this ]() { return isWriteAllowed(); } );

			size_t currentWriteIdx		 = _writeIdx;
			_circBuff[ currentWriteIdx ] = std::move( elem );
			// VTX_INFO( "WriteElement currentWriteIdx {}", currentWriteIdx );
			updateWriteIdx();

			unique_lock.unlock();
			_read_allowed.notify_one();

			return _circBuff[ currentWriteIdx ];
		}
		/**
		 * @brief Get the element at the current read index and increments read index.
		 */
		bool readElement( T & elem )
		{
			std::unique_lock<std::mutex> unique_lock( _access_mtx );
			_read_allowed.wait( unique_lock, [ this ]() { return isReadAllowed(); } );

			elem = _circBuff[ _readIdx ];
			// VTX_INFO( "ReadElement readIdx {}", _readIdx );
			updateReadIdx();

			unique_lock.unlock();
			_write_allowed.notify_one();

			return true;
		}
		T & readElement( void )
		{
			std::unique_lock<std::mutex> unique_lock( _access_mtx );
			_read_allowed.wait( unique_lock, [ this ]() { return isReadAllowed(); } );

			updateReadIdx();

			unique_lock.unlock();
			_write_allowed.notify_one();

			return _circBuff[ _readIdx ];
		}

		size_t getBuffSize( void ) const { return _buffSize; }

		/**
		 * @brief Resets read and write indexes of the circular buffer.
		 * Data holded in the buffer is not wiped as read and write resets ensures that the buffer will be reused.
		 * A memory optimization could be wiping the content of the buffer.
		 */
		void reset( void )
		{
			_readIdx  = 0;
			_writeIdx = 0;
		}

	  protected:
		/**
		 * @brief Get the element at the requested read index and does not change read index.
		 */
		const T & getElement( size_t index ) const { return _circBuff[ index % _buffSize ]; }
		T &		  getElement( size_t index ) { return _circBuff[ index % _buffSize ]; }

		/**
		 * @brief Updates write index and overflowflag if a loop back has happened
		 */
		void updateWriteIdx( void )
		{
			_writeIdx >= _buffSize - 1 ? _writeIdx = 0 : _writeIdx++;
			if ( !_writeIdx )
				_overflowFlag ^= circbuffmask_write_overflow;
		}
		/**
		 * @brief Updates read index and overflowflag if a loop back has happened
		 */
		void updateReadIdx( void )
		{
			_readIdx >= _buffSize - 1 ? _readIdx = 0 : _readIdx++;
			if ( !_readIdx )
				_overflowFlag ^= circbuffmask_read_overflow;
		}
		void setBuffSize( size_t size )
		{
			_circBuff.resize( size );
			_buffSize = size;
		}
		size_t getReadIdx( void ) const { return _readIdx; }
		size_t getWriteIdx( void ) const { return _writeIdx; }

	  private:
		size_t		   _buffSize;
		size_t		   _readIdx;
		size_t		   _writeIdx;
		std::vector<T> _circBuff;
		char		   _overflowFlag;
		// concurency
		std::mutex				_access_mtx;
		std::condition_variable _write_allowed;
		std::condition_variable _read_allowed;

		/**
		 * @brief Ensures that read operation is performed on continuous data or skips one buffer.
		 * Write index should alway be heading read index except if write index has reached the end of the buffer
		 * and has looped back. The loop back aspect is handled by the overflow flag, a simple bit to handle two states
		 * (looped back or not).
		 * The overflow flag is updated in read and write operations and is readonly here.
		 */
		bool isReadAllowed( void ) const
		{
			bool allowed( false );

			if ( 0 == _buffSize )
				return allowed;

			if ( (bool)( _overflowFlag & circbuffmask_read_overflow )
				 == (bool)( _overflowFlag & circbuffmask_write_overflow ) )
				allowed = _readIdx < _writeIdx;
			else
				allowed = _readIdx >= _writeIdx;

			return allowed;
		}

		/**
		 * @brief Write operation always allowed.
		 */
		bool isWriteAllowed( void ) const { return true; }
	};
} // namespace VTX::Core::Struct

#endif
