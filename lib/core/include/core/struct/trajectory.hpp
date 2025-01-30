#ifndef __VTX_CORE_STRUCT_TRAJECTORY__
#define __VTX_CORE_STRUCT_TRAJECTORY__

#include <core/struct/frames_data_circbuff_prodcons.hpp>
#include <core/struct/frames_data_vector.hpp>
#include <util/types.hpp>
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
			_isOptimized	 = movable.isOptimized();
			_framesOptimized = std::move( movable._framesOptimized );
			_framesPlain	 = std::move( movable._framesPlain );
		}
		Trajectory & operator=( Trajectory && movable )
		{
			_isOptimized	 = movable.isOptimized();
			_framesOptimized = std::move( movable._framesOptimized );
			_framesPlain	 = std::move( movable._framesPlain );
			return *this;
		}
		// Trajectory ( Trajectory & copy ) = delete;

		void fillFrame( const size_t p_systemFrameIndex, const std::vector<Vec3f> & p_atomPositions )
		{
			/* Frame frame;
			frame.resize( p_atomPositions.size() );
			std::copy( p_atomPositions.begin(), p_atomPositions.end(), frame.begin() );
			_framesCircBuff.WriteElement( frame );*/

			if ( _isOptimized )
			{
				Frame frame;
				frame.resize( p_atomPositions.size() );
				std::copy( p_atomPositions.begin(), p_atomPositions.end(), frame.begin() );
				_framesOptimized.writeElement( frame );
			}
			else
				_framesPlain.fillFrame( p_systemFrameIndex, p_atomPositions );
		}

		// devjla DEBUG TRAJECTORY FRAMES ORDER
		void fillFrameDEBUG()
		{
			if ( _isOptimized )
			{
				for ( float x = -100.f; x <= 100.f; x += 50.f )
				{
					for ( float y = -100.f; y <= 100.f; y += 50.f )
					{
						Frame					 frame;
						const std::vector<Vec3f> atomPositionsDebug { { x, y, 0 } };
						frame.resize( atomPositionsDebug.size() );
						std::copy( atomPositionsDebug.begin(), atomPositionsDebug.end(), frame.begin() );
						_framesOptimized.writeElement( frame );
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
						_framesPlain.fillFrame( idx, atomPositionsDebug );
						++idx;
					}
				}
			}
		}

		// dynamicMemoryUsage requirement
		// needs refacto to handle circular buffers
		const FramesDataVector & getFramesPlain() const { return _framesPlain; }

		const Frame & getCurrentFrame() const
		{
			if ( _isOptimized )
				return _framesOptimized.getCurrentFrame();
			else
				return _framesPlain.getCurrentFrame();
		}
		Frame & getCurrentFrame()
		{
			if ( _isOptimized )
				return _framesOptimized.getCurrentFrame();
			else
				return _framesPlain.getCurrentFrame();
		}

		// not available for optimized trajectories
		void setCurrentFrameIndex( size_t p_currentFrameIdx )
		{
			if ( _isOptimized )
				return;
			_framesPlain.setCurrentFrameIndex( p_currentFrameIdx );
		}
		// not available for optimized trajectories
		const size_t getCurrentFrameIndex( void ) const
		{
			if ( _isOptimized )
				return (size_t)( -1 );
			return _framesPlain.getCurrentFrameIndex();
		}

		// not available for optimized trajectories
		const Frame & getFrameFromIndex( size_t p_index ) const { return _framesPlain.getFrameFromIndex( p_index ); }
		Frame &		  getFrameFromIndex( size_t p_index ) { return _framesPlain.getFrameFromIndex( p_index ); }

		const size_t getFrameCount() const
		{
			if ( _isOptimized )
				return _framesOptimized.getTotalElements();
			else
				return _framesPlain.getTotalElements();
		}
		void setTotalElements( const size_t size )
		{
			if ( _isOptimized )
				_framesOptimized.setTotalElements( size );
			else
				_framesPlain.setTotalElements( size );
		}

		const bool isOptimized( void ) const { return _isOptimized; }
		void	   setOptimized( void ) { _isOptimized = true; }

		// not available for plain trajectories (non-optimized)
		Frame & readOptimizedElement( void ) { return _framesOptimized.readElement(); }
		// not available for plain trajectories (non-optimized)
		void reset( void ) { _framesOptimized.reset(); }

		void eraseEmptyFrames( void )
		{
			if ( _isOptimized )
				_framesOptimized.eraseEmptyFrames();
			else
				_framesPlain.eraseEmptyFrames();
		}

	  private:
		bool					   _isOptimized = false;
		FramesDataCircBuffProdCons _framesOptimized;
		FramesDataVector		   _framesPlain;
	};

	ByteNumber dynamicMemoryUsage( const Trajectory & ) noexcept;

} // namespace VTX::Core::Struct

#endif
