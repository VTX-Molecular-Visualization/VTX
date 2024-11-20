#ifndef __VTX_CORE_STRUCT_TRAJECTORY__
#define __VTX_CORE_STRUCT_TRAJECTORY__

#include <util/types.hpp>
#include <core/struct/frames_data_circbuff_prodcons.hpp>
#include <core/struct/frames_data_vector.hpp>
#include <vector>

namespace VTX::Core::Struct
{
	/**
	 * @brief Each index is correlated with other vector's that contains atom's data
	 */
	using Frame = std::vector<Vec3f>;
	class Trajectory final
	{
	  public:
		// FIXME really?
		Trajectory() {}
		Trajectory( Trajectory && movable )
		{
			_isOptimized	   = movable.IsOptimized();
			_framesOptimized	  = std::move( movable._framesOptimized );
			_framesPlain	   = std::move( movable._framesPlain );
		}
		Trajectory & operator=( Trajectory && movable )
		{
			_isOptimized	   = movable.IsOptimized();
			_framesOptimized	  = std::move( movable._framesOptimized );
			_framesPlain	   = std::move( movable._framesPlain );
			return *this;
		}
		//Trajectory ( Trajectory & copy ) = delete;

		void FillFrame( const size_t p_systemFrameIndex, const std::vector<Vec3f> & p_atomPositions )
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
				_framesOptimized.WriteElement( frame );
			}
			else
				_framesPlain.FillFrame( p_systemFrameIndex, p_atomPositions );
		}

		// devjla DEBUG TRAJECTORY FRAMES ORDER
		void FillFrameDEBUG()
		{
			if ( _isOptimized )
			{	
				for ( float x = -100.f; x <= 100.f; x += 50.f )
				{
					for ( float y = -100.f; y <= 100.f; y += 50.f )
					{
						Frame frame;
						const std::vector<Vec3f> atomPositionsDebug { { x, y, 0 } };
						frame.resize( atomPositionsDebug.size() );
						std::copy( atomPositionsDebug.begin(), atomPositionsDebug.end(), frame.begin() );
						_framesOptimized.WriteElement( frame );
					}
				}
			}
			else
			{
				size_t idx( 0 );
				for ( float x = -100.f; x <= 100.f; x += 50.f )
				{
					for ( float y = -100.f; y <= 100.f; y += 50.f )
					{
						const std::vector<Vec3f> atomPositionsDebug { { x, y, 0 } };
						_framesPlain.FillFrame( idx, atomPositionsDebug );
						++idx;
					}
				}
			}
		}

		const Frame & GetCurrentFrame() const 
		{
			if ( _isOptimized )
				return _framesOptimized.GetCurrentFrame();
			else
				return _framesPlain.GetCurrentFrame();
		}
		Frame & GetCurrentFrame()
		{
			if ( _isOptimized )
				return _framesOptimized.GetCurrentFrame();
			else
				return _framesPlain.GetCurrentFrame();
		}

		// not available for optimized trajectories
		void SetCurrentFrameIndex( size_t p_currentFrameIdx )
		{
			if ( _isOptimized )
				return;
			_framesPlain.SetCurrentFrameIndex( p_currentFrameIdx );
		}
		// not available for optimized trajectories
		const size_t GetCurrentFrameIndex( void ) const
		{
			if ( _isOptimized )
				return (size_t)(-1);
			return _framesPlain.GetCurrentFrameIndex();
		}

		// not available for optimized trajectories
		const Frame & GetFrameFromIndex( size_t p_index ) const {return _framesPlain.GetFrameFromIndex( p_index ); }
		Frame &		  GetFrameFromIndex( size_t p_index ) { return _framesPlain.GetFrameFromIndex( p_index ); }

		const size_t GetFrameCount() const
		{
			if ( _isOptimized )
				return _framesOptimized.GetTotalElements();
			else
				return _framesPlain.GetTotalElements();
		}
		void   SetTotalElements( const size_t size )
		{ 
			if ( _isOptimized )
				_framesOptimized.SetTotalElements( size );
			else
				_framesPlain.SetTotalElements( size );
		}

		const bool IsOptimized( void ) const { return _isOptimized; }
		void	   SetOptimized( void ) { _isOptimized = true; }
		
		// not available for plain trajectories (non-optimized)
		Frame & ReadOptimizedElement( void ) { return _framesOptimized.ReadElement(); }
		// not available for plain trajectories (non-optimized)
		void Reset( void ) { _framesOptimized.Reset(); }

		void EraseEmptyFrames( void )
		{
			if ( _isOptimized )
				_framesOptimized.EraseEmptyFrames();
			else
				_framesPlain.EraseEmptyFrames();
		}

		private:
		bool			   _isOptimized = false;
		FramesDataCircBuffProdCons _framesOptimized;
		FramesDataVector		   _framesPlain;
	};

	ByteNumber dynamicMemoryUsage( const Trajectory & ) noexcept;

} // namespace VTX::Core::Struct

#endif
