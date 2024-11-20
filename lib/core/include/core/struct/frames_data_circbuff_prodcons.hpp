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
		FramesDataCircBuffProdCons() : _totalElements( 0 )
		{
			SetBuffSize( 102 );
		}
		// FIXME really?
		FramesDataCircBuffProdCons( FramesDataCircBuffProdCons && movable )
		{
			ProdConsCircularBuffer<Frame>::SetBuffSize( movable.GetBuffSize() );
			_totalElements = movable._totalElements;
		}
		FramesDataCircBuffProdCons & operator=( const FramesDataCircBuffProdCons && movable )
		{
			ProdConsCircularBuffer<Frame>::operator=( std::move(movable) );
			_totalElements = movable._totalElements;
			return *this;
		}
		Frame & WriteElement( const Frame & elem )
		{
			return ProdConsCircularBuffer<Frame>::WriteElement( elem );
		}
		bool ReadElement( Frame & elem )
		{
			return ProdConsCircularBuffer<Frame>::ReadElement(elem);
		}
		Frame & ReadElement( void )
		{
			return ProdConsCircularBuffer<Frame>::ReadElement();
		}
		void	SetTotalElements( const size_t size ) { _totalElements = size; }
		
		size_t GetTotalElements( void ) const { return _totalElements; }

		//Frame & GetModelFrame( void ) { return GetElement( 0 ); }
		//const Frame & GetModelFrame( void ) const { return GetElement( 0 ); }
		
		// Reads current frame data but does not change read index
		//Frame & GetCurrentFrame( void ) { return GetElement( GetReadIdx() ); }
		const Frame & GetCurrentFrame( void ) const { return GetElement( GetReadIdx() ); }
		Frame & GetCurrentFrame( void ) { return GetElement( GetReadIdx() ); }
		

		void Reset( void ) { ProdConsCircularBuffer<Frame>::Reset(); }

		// FIXME nothing to do here ?
		void EraseEmptyFrames( void ) {}

	  private:
		size_t				 _totalElements; // TODO change to double if possible
	};
} // namespace VTX::Core::Struct

#endif
