#ifndef __VTX_APP_CORE_PLAYER_CIRCULAR_BUFFER__
#define __VTX_APP_CORE_PLAYER_CIRCULAR_BUFFER__

#include <app/core/player/base_player.hpp>
#include <app/core/threading/base_thread.hpp>
#include <core/struct/frames_to_render.hpp>

namespace VTX::App::Core::Player
{
	class CircularBuffer : public BasePlayer
	{
	  public:
		inline static const std::string DISPLAYED_NAME = "CircularBuffer";

	  public:
		CircularBuffer()								  = default;
		CircularBuffer( const CircularBuffer & p_source ) = default;
		virtual ~CircularBuffer()						  = default;

		const std::string & getDisplayName() const override { return DISPLAYED_NAME; }

		void reset() override;
		void nextFrame( const size_t frameCount = 1 ) override;

		void update( const float, const float );

		/**
		 * @brief Creates two threads managed by the ThreadingSystem.
		 * First thread reads trajectory file and writes circular buffer data.
		 * Second thread reads circular buffer data and writes Frames to be displayed in a FramesToRender object (queue
		 * container).
		 */
		void play() override;
		void pause() override;
		void stop() override;

		/**
		 * @brief Copy a frame to be displayed into a FramesToRender object (queue container).
		 * FIXME can be optimized by not taking a copy?
		 */
		void stackFrame( VTX::Core::Struct::Frame elem ) { _displayFrames.addElement( elem ); }
		/**
		 * @brief Get a copy of the next frame to be displayed to the user.
		 * Using a copy to ensure we do not erase the frame in the lifo queue before it is processed by the engine.
		 * FIXME can be optimized by not taking a copy?
		 */
		bool getCopyFrame( VTX::Core::Struct::Frame & frame ) { return _displayFrames.getCopyFrame( frame ); }

		/**
		 * @brief Index to update progress slider in UI.
		 */
		const size_t getIndex( void ) const { return _displayFrames.getindex(); }

	  private:
		VTX::Core::Struct::FramesToRender  _displayFrames;
		App::Core::Threading::BaseThread * _readThread	= nullptr;
		App::Core::Threading::BaseThread * _writeThread = nullptr;
	};
} // namespace VTX::App::Core::Player
#endif
