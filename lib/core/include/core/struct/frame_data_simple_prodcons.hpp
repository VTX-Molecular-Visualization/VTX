#ifndef __VTX_CORE_STRUCT_FRAMEBUFFSIMPLE_PRODCONS__
#define __VTX_CORE_STRUCT_FRAMEBUFFSIMPLE_PRODCONS__

#include "circular_buffer_prodcons.hpp"
#include <util/types.hpp>

namespace VTX::Core::Struct
{
	using Frame = std::vector<Vec3f>;

	class FrameDataProdCons : public ProdConsCircularBuffer<Frame>
	{
	  public:
		FrameDataProdCons() : totalElements( 0 )
		{
			SetBuffSize( 102 );
		}
		// FIXME really?
		FrameDataProdCons( FrameDataProdCons && movable )
		{
			ProdConsCircularBuffer<Frame>::SetBuffSize( movable.GetBuffSize() );
			totalElements = movable.totalElements;
		}
		FrameDataProdCons & operator=( const FrameDataProdCons && movable )
		{
			ProdConsCircularBuffer<Frame>::operator=( std::move(movable) );
			totalElements = movable.totalElements;
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
		void	SetTotalElements( const size_t size ) { totalElements = size; }
		
		size_t GetTotalElements( void ) const { return totalElements; }

		Frame & GetModelFrame( void ) { return GetElement( 0 ); }
		const Frame & GetModelFrame( void ) const { return GetElement( 0 ); }
		
		// Reads current frame data but does not change read index
		Frame & GetCurrentFrame( void ) { return GetElement( GetReadIdx() ); }

		void Reset( void ) { ProdConsCircularBuffer<Frame>::Reset(); }

	  private:
		size_t				 totalElements; // TODO change to double if possible
	};
} // namespace VTX::Core::Struct

#endif
