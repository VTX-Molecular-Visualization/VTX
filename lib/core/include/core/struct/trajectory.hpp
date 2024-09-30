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
			frames = std::move(movable.frames);
			currentFrameIndex = movable.currentFrameIndex;
		}
		Trajectory & operator=( Trajectory && movable )
		{
			frames			  = std::move( movable.frames );
			currentFrameIndex = movable.currentFrameIndex;
			return *this;
		}
		//Trajectory ( Trajectory & copy ) = delete;
		void fillFrame( const size_t p_systemFrameIndex, const std::vector<Vec3f> & p_atomPositions )
		{
			/*
			Frame	initFrame;
			Frame & frame = frames.WriteElement( initFrame );
			frame.resize( p_atomPositions.size() );

			std::copy( p_atomPositions.begin(), p_atomPositions.end(), frame.begin() );
			*/
			Frame frame;
			frame.resize( p_atomPositions.size() );
			std::copy( p_atomPositions.begin(), p_atomPositions.end(), frame.begin() );
			frames.WriteElement( frame );
		}

		// devjla
		/* const Frame & getCurrentFrame() const { return frames[ currentFrameIndex ]; }
		Frame &		  getCurrentFrame() { return frames[ currentFrameIndex ]; } */
		/* const Frame & getCurrentFrame() const
		{
			Frame frame;
			if ( frames.ReadElement( frame ) )
				return frame;
		} */
		bool getCurrentFrame( Frame & requestedFrame )
		{
			if ( frames.ReadElement( requestedFrame ) )
				return true;
			else
				return false;
		}

		Frame& getModelFrame(void)
		{ 
			return frames.GetModelFrame();
		}

		//size_t getFrameCount() const { return frames.size(); }
		size_t getFrameCount() const { return frames.GetTotalElements(); }

		//std::vector<Frame> frames;
		//FrameDataSimple frames;
		FrameDataProdCons frames;
		size_t			  currentFrameIndex = 0;
	};

	ByteNumber dynamicMemoryUsage( const Trajectory & ) noexcept;

} // namespace VTX::Core::Struct

#endif
