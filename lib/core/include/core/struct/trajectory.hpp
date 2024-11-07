#ifndef __VTX_CORE_STRUCT_TRAJECTORY__
#define __VTX_CORE_STRUCT_TRAJECTORY__

#include <util/types.hpp>
//#include <core/struct/frame_data.hpp>
//#include <core/struct/frame_data_simple.hpp>
#include <core/struct/frame_data_simple_prodcons.hpp>
#include <vector>

namespace VTX::Core::Struct
{
	/**
	 * @brief Each index is correlated with other vector's that contains atom's data
	 */
	using Frame = std::vector<Vec3f>;
	struct Trajectory
	{
		// FIXME really?
		Trajectory() {}
		Trajectory( Trajectory && movable )
		{
			_framesCircBuff	  = std::move( movable._framesCircBuff );
			_currentFrameIndex = movable._currentFrameIndex;
			_isOptimized	   = movable._isOptimized;
			_framesVector	   = std::move( movable._framesVector );
		}
		Trajectory & operator=( Trajectory && movable )
		{
			_framesCircBuff	  = std::move( movable._framesCircBuff );
			_currentFrameIndex = movable._currentFrameIndex;
			_isOptimized	   = movable._isOptimized;
			_framesVector	   = std::move( movable._framesVector );
			return *this;
		}
		//Trajectory ( Trajectory & copy ) = delete;

		void fillFrame( const size_t p_systemFrameIndex, const std::vector<Vec3f> & p_atomPositions )
		{
			/* Frame frame;
			frame.resize( p_atomPositions.size() );
			std::copy( p_atomPositions.begin(), p_atomPositions.end(), frame.begin() );
			_framesCircBuff.WriteElement( frame );*/

			if (_isOptimized)
			{
				Frame frame;
				frame.resize( p_atomPositions.size() );
				std::copy( p_atomPositions.begin(), p_atomPositions.end(), frame.begin() );
				_framesCircBuff.WriteElement( frame );
			}
			else
			{
				//Frame frame;
				//frame.resize( p_atomPositions.size() );
				//_framesVector.push_back( frame );
				//std::copy( p_atomPositions.begin(), p_atomPositions.end(), frame.begin() );
				_framesVector[ p_systemFrameIndex ].resize( p_atomPositions.size() );
				std::copy(
					p_atomPositions.begin(), p_atomPositions.end(), _framesVector[ p_systemFrameIndex ].begin()
				);
			}
		}

		// devjla
		const Frame & GetCurrentFrame() const 
		{
			if ( _isOptimized )
				return _framesCircBuff.GetCurrentFrame();
			else
				return _framesVector[ _currentFrameIndex ]; 
		}
		Frame & GetCurrentFrame()
		{
			if ( _isOptimized )
				return _framesCircBuff.GetCurrentFrame();
			else
				return _framesVector[ _currentFrameIndex ];
		}
		/* const Frame & getCurrentFrame() const
		{
			Frame frame;
			if ( frames.ReadElement( frame ) )
				return frame;
		} */
		/* bool getCurrentFrame( Frame & requestedFrame )
		{
			if ( _framesCircBuff.ReadElement( requestedFrame ) )
				return true;
			else
				return false;
		}*/

		/* Frame & getModelFrame( void )
		{ 
			return _framesCircBuff.GetModelFrame();
		}*/

		//size_t getFrameCount() const { return frames.size(); }
		size_t GetFrameCount() const
		{
			if ( _isOptimized )
				return _framesCircBuff.GetTotalElements();
			else
				return _framesVector.size();
		}
		void   SetTotalElements( const size_t size )
		{ 
			if ( _isOptimized )
				_framesCircBuff.SetTotalElements( size );
			else
				_framesVector.resize( size );
		}

		void Reset( void ) { _framesCircBuff.Reset(); }

		bool			   _isOptimized = false;
		std::vector<Frame> _framesVector;
		//FrameDataSimple frames;
		FrameDataProdCons _framesCircBuff;
		size_t			  _currentFrameIndex = 0;
	};

	ByteNumber dynamicMemoryUsage( const Trajectory & ) noexcept;

} // namespace VTX::Core::Struct

#endif
