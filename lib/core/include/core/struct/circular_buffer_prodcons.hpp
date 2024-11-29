#ifndef __VTX_CORE_STRUCT_CIRCBUFF_PRODCONS__
#define __VTX_CORE_STRUCT_CIRCBUFF_PRODCONS__

#include <util/logger.hpp>
#include <vector>

namespace VTX::Core::Struct
{
	constexpr char tmp_circbuffmask_read_overflow { 0x1 };
	constexpr char tmp_circbuffmask_write_overflow { 0x2 };

	template<typename T>
	class ProdConsCircularBuffer
	{
	  public:
		ProdConsCircularBuffer() : _readIdx( 0 ), _writeIdx( 0 ), _buffSize( 0 ), _overflowFlag( 0 )
		{
			setBuffSize( 0 );
		}
		// Currently this ctor is not used
		// Call UpdateBuffSize method to define buffer size
		ProdConsCircularBuffer( const size_t size ) :
			_readIdx( 0 ), _writeIdx( 0 ), _buffSize( size ), _overflowFlag( 0 )
		{
			setBuffSize( size );
			return;
		}
		// FIXME really?
		ProdConsCircularBuffer( ProdConsCircularBuffer && movable ) // : circBuff( std::move( movable.circBuff ) )
		// mutex is default initialized?
		{
			_circBuff	  = std::move( movable._circBuff );
			_readIdx	  = movable._readIdx;
			_writeIdx	  = movable._writeIdx;
			_buffSize	  = movable._buffSize;
			_overflowFlag = movable._overflowFlag;
		}
		ProdConsCircularBuffer & operator=( const ProdConsCircularBuffer && movable ) noexcept
		{
			_circBuff	  = std::move( movable._circBuff );
			_readIdx	  = movable._readIdx;
			_writeIdx	  = movable._writeIdx;
			_buffSize	  = movable._buffSize;
			_overflowFlag = movable._overflowFlag;
			return *this;
		}
		T & writeElement( const T & elem )
		{
			std::unique_lock<std::mutex> unique_lock( _access_mtx );
			_write_allowed.wait( unique_lock, [ this ]() { return isWriteAllowed(); } );

			size_t currentWriteIdx		 = _writeIdx;
			_circBuff[ currentWriteIdx ] = std::move( elem );
			VTX_INFO( "WriteElement currentWriteIdx {}", currentWriteIdx );
			updateWriteIdx();

			unique_lock.unlock();
			_read_allowed.notify_one();

			return _circBuff[ currentWriteIdx ];
		}
		bool readElement( T & elem )
		{
			std::unique_lock<std::mutex> unique_lock( _access_mtx );
			_read_allowed.wait( unique_lock, [ this ]() { return isReadAllowed(); } );

			elem = _circBuff[ _readIdx ];
			VTX_INFO( "ReadElement readIdx {}", _readIdx );
			updateReadIdx();

			unique_lock.unlock();
			_write_allowed.notify_one();

			return true;
		}
		// FIXME where is it used?
		T & readElement( void )
		{
			std::unique_lock<std::mutex> unique_lock( _access_mtx );
			_read_allowed.wait( unique_lock, [ this ]() { return isReadAllowed(); } );

			updateReadIdx();

			unique_lock.unlock();
			_write_allowed.notify_one();

			return _circBuff[ _readIdx ];
		}
		void eraseEmptyFrames( void )
		{
			/* do
			{
				trajectory.frames.pop_back();
			} while ( trajectory.frames.back().size() == 0 ); */
			// TODO
			return;
		}
		size_t getBuffSize( void ) const { return _buffSize; }

		void reset( void )
		{
			// FIXME reset also the content of the buffer?
			_readIdx  = 0;
			_writeIdx = 0;
		}

	  protected:
		// FIXME
		const T & getElement( size_t index ) const { return _circBuff[ index % _buffSize ]; }
		T &		  getElement( size_t index ) { return _circBuff[ index % _buffSize ]; }

		void updateWriteIdx( void )
		{
			_writeIdx >= _buffSize - 1 ? _writeIdx = 0 : _writeIdx++;
			if ( !_writeIdx )
				_overflowFlag ^= tmp_circbuffmask_write_overflow;
		}
		void updateReadIdx( void )
		{
			_readIdx >= _buffSize - 1 ? _readIdx = 0 : _readIdx++;
			if ( !_readIdx )
				_overflowFlag ^= tmp_circbuffmask_read_overflow;
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

		bool isReadAllowed( void ) const
		{
			bool allowed( false );

			if ( 0 == _buffSize )
				return allowed;

			if ( (bool)( _overflowFlag & tmp_circbuffmask_read_overflow )
				 == (bool)( _overflowFlag & tmp_circbuffmask_write_overflow ) )
				allowed = _readIdx < _writeIdx;
			else
				allowed = _readIdx >= _writeIdx;

			return allowed;
		}

		bool isWriteAllowed( void ) const
		{
			// return !isReadAllowed();
			return true;
		}
	};
} // namespace VTX::Core::Struct

#endif
