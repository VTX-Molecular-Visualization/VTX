#ifndef __VTX_CORE_STRUCT_FRAMES_DATA_CIRCBUFFPRODCONS__
#define __VTX_CORE_STRUCT_FRAMES_DATA_CIRCBUFFPRODCONS__

#include "circular_buffer_prodcons.hpp"
#include <util/types.hpp>

namespace VTX::Core::Struct
{
	using Frame = std::vector<Vec3f>;

	class FramesDataCircBuffProdCons : public ProdConsCircularBuffer<Frame>
	{
	  public:
		FramesDataCircBuffProdCons() : _totalElements( 0 ) { setBuffSize( 102 ); }
		// FIXME really?
		FramesDataCircBuffProdCons( FramesDataCircBuffProdCons && movable )
		{
			ProdConsCircularBuffer<Frame>::setBuffSize( movable.getBuffSize() );
			_totalElements = movable._totalElements;
		}
		FramesDataCircBuffProdCons & operator=( const FramesDataCircBuffProdCons && movable )
		{
			ProdConsCircularBuffer<Frame>::operator=( std::move( movable ) );
			_totalElements = movable._totalElements;
			return *this;
		}
		Frame & writeElement( const Frame & elem ) { return ProdConsCircularBuffer<Frame>::writeElement( elem ); }
		bool	readElement( Frame & elem ) { return ProdConsCircularBuffer<Frame>::readElement( elem ); }
		Frame & readElement( void ) { return ProdConsCircularBuffer<Frame>::readElement(); }
		void	setTotalElements( const size_t size ) { _totalElements = size; }

		size_t getTotalElements( void ) const { return _totalElements; }

		// Frame & GetModelFrame( void ) { return GetElement( 0 ); }
		// const Frame & GetModelFrame( void ) const { return GetElement( 0 ); }

		// Reads current frame data but does not change read index
		// Frame & GetCurrentFrame( void ) { return GetElement( GetReadIdx() ); }
		const Frame & getCurrentFrame( void ) const { return getElement( getReadIdx() ); }
		Frame &		  getCurrentFrame( void ) { return getElement( getReadIdx() ); }

		void reset( void ) { ProdConsCircularBuffer<Frame>::reset(); }

		// FIXME nothing to do here ?
		void eraseEmptyFrames( void ) {}

	  private:
		size_t _totalElements; // TODO change to double if possible
	};
} // namespace VTX::Core::Struct

#endif
