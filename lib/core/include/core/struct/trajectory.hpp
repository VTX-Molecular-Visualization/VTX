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
	 * Trajectory is responsible of two types of data containers
	 * _framesOptimized is a holder for a circular buffer
	 * _framesPlain is a holder for a vector
	 * Either one or the other is used and the other one remains empty
	 * All methods check for the optimization boolean _isOptimized to decide which holder they must access to do their
	 * tasks
	 */
	using Frame = std::vector<Vec3f>;
	class Trajectory final
	{
	  public:
		Trajectory() {}
		/**
		 * @brief Move ctor explicit because FramesDataCircBuffProdCons members section has mutexes.
		 */
		Trajectory( Trajectory && movable )
		{
			_isOptimized	 = movable.isOptimized();
			_framesOptimized = std::move( movable._framesOptimized );
			_framesPlain	 = std::move( movable._framesPlain );
		}
		/**
		 * @brief Move assignement operator explicit because FramesDataCircBuffProdCons members section has mutexes.
		 */
		Trajectory & operator=( Trajectory && movable )
		{
			_isOptimized	 = movable.isOptimized();
			_framesOptimized = std::move( movable._framesOptimized );
			_framesPlain	 = std::move( movable._framesPlain );
			return *this;
		}

		/**
		 * @brief Copy input frame p_atomPositions in the dedicated trajectory data holder.
		 */
		void fillFrame( const size_t p_systemFrameIndex, const std::vector<Vec3f> & p_atomPositions )
		{
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

		/**
		 * @brief dynamicMemoryUsage requirement
		 * FIXME needs refacto to handle circular buffers
		 */
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

		/**
		 * @brief Sets the current frame index for the vector trajectory holder.
		 * Not available for optimized trajectories (circular buffer holder).
		 */
		void setCurrentFrameIndex( size_t p_currentFrameIdx )
		{
			if ( _isOptimized )
				return;
			_framesPlain.setCurrentFrameIndex( p_currentFrameIdx );
		}
		/**
		 * @brief Gets the current frame index for the vector trajectory holder.
		 * Not available for optimized trajectories (circular buffer holder).
		 */
		const size_t getCurrentFrameIndex( void ) const
		{
			if ( _isOptimized )
				return (size_t)( -1 );
			return _framesPlain.getCurrentFrameIndex();
		}

		/**
		 * @brief Gets the current frame for the vector trajectory holder.
		 * Not available for optimized trajectories (circular buffer holder).
		 * FIXME no checks of optimization state done !
		 */
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

		/**
		 * @brief Reads the current frame for the circular buffer trajectory holder.
		 * Not available for non-optimized trajectories (vector holder).
		 * FIXME no checks of optimization state done !
		 */
		Frame & readOptimizedElement( void ) { return _framesOptimized.readElement(); }
		/**
		 * @brief Resets read and write indexes of the circular buffer trajectory holder.
		 * Not available for non-optimized trajectories (vector holder).
		 * FIXME no checks of optimization state done !
		 */
		void reset( void ) { _framesOptimized.reset(); }

		void eraseEmptyFrames( void ) { _framesPlain.eraseEmptyFrames(); }

	  private:
		bool					   _isOptimized = false;
		FramesDataCircBuffProdCons _framesOptimized;
		FramesDataVector		   _framesPlain;
	};

	ByteNumber dynamicMemoryUsage( const Trajectory & ) noexcept;

} // namespace VTX::Core::Struct

#endif
