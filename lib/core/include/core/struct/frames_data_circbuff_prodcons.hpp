#ifndef __VTX_CORE_STRUCT_FRAMES_DATA_CIRCBUFFPRODCONS__
#define __VTX_CORE_STRUCT_FRAMES_DATA_CIRCBUFFPRODCONS__

#include "circular_buffer_prodcons.hpp"
#include <util/types.hpp>

namespace VTX::Core::Struct
{
	using Frame = std::vector<Vec3f>;

	/**
	 * @brief Holder of an optimized trajectory by specifying a generic circular buffer storing Frame elements.
	 */
	class FramesDataCircBuffProdCons : public ProdConsCircularBuffer<Frame>
	{
	  public:
		FramesDataCircBuffProdCons() : _totalElements( 0 ) { setBuffSize( 102 ); }
		/**
		 * @brief Move ctor explicit because ProdConsCircularBuffer member section has mutexes.
		 */
		FramesDataCircBuffProdCons( FramesDataCircBuffProdCons && movable )
		{
			ProdConsCircularBuffer<Frame>::setBuffSize( movable.getBuffSize() );
			_totalElements = movable._totalElements;
		}
		/**
		 * @brief Move assignement operator explicit because FramesDataCircBuffProdCons members section has mutexes.
		 */
		FramesDataCircBuffProdCons & operator=( const FramesDataCircBuffProdCons && movable )
		{
			ProdConsCircularBuffer<Frame>::operator=( std::move( movable ) );
			_totalElements = movable._totalElements;
			return *this;
		}
		/**
		 * @brief Moves the content of frame element into the circular buffer at the current write index and increments
		 * write index.
		 */
		Frame & writeElement( const Frame & elem ) { return ProdConsCircularBuffer<Frame>::writeElement( elem ); }
		/**
		 * @brief Get the frame element at the current read index and increments read index.
		 */
		bool	readElement( Frame & elem ) { return ProdConsCircularBuffer<Frame>::readElement( elem ); }
		Frame & readElement( void ) { return ProdConsCircularBuffer<Frame>::readElement(); }
		void	setTotalElements( const size_t size ) { _totalElements = size; }

		size_t getTotalElements( void ) const { return _totalElements; }

		/**
		 * @brief Get the frame element at the current read index but does not change read index
		 */
		const Frame & getCurrentFrame( void ) const { return getElement( getReadIdx() ); }
		Frame &		  getCurrentFrame( void ) { return getElement( getReadIdx() ); }

		/**
		 * @brief Resets read and write indexes of the circular buffer.
		 */
		void reset( void ) { ProdConsCircularBuffer<Frame>::reset(); }

	  private:
		size_t _totalElements;
	};
} // namespace VTX::Core::Struct

#endif
