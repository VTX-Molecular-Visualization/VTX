#ifndef __VTX_APP_CORE_PLAYER_CIRCULAR_BUFFER__
#define __VTX_APP_CORE_PLAYER_CIRCULAR_BUFFER__

#include <app/core/player/base_player.hpp>
#include <app/core/player/players.hpp>
#include <core/struct/frames_to_render.hpp>
#include <app/core/threading/base_thread.hpp>

namespace VTX::App::Core::Player
{
	class CircularBuffer : public BasePlayer
	{
	  public:
		inline static const std::string DISPLAYED_NAME = "CircularBuffer";

	  public:
		CircularBuffer()						  = default;
		CircularBuffer( const CircularBuffer & p_source ) = default;
		virtual ~CircularBuffer() = default;

		const std::string & getDisplayName() const override { return DISPLAYED_NAME; }

		void reset() override;
		void nextFrame( const size_t frameCount = 1 ) override;
		
		void update( const float p_deltaTime ) override;

		void play() override;
		void pause() override;
		void stop() override;
		
		void StackFrame( VTX::Core::Struct::Frame elem ) { _tmpFrames.AddElement( elem ); } // FIXME taking a copy as input?
		bool GetRefFrame( VTX::Core::Struct::Frame &frame ) { return _tmpFrames.ReadElement(frame); }
		void RemoveRefFrame( VTX::Core::Struct::Frame & elem ) { _tmpFrames.RemoveElement( elem ); }
		bool GetCopyFrame( VTX::Core::Struct::Frame &frame ) { return _tmpFrames.GetCopyFrame(frame); }

		// FIXME test to update progress slider in UI
		const size_t getIndex( void ) const { return _tmpFrames.getindex(); }
		
	  private:
		//devjla
		VTX::Core::Struct::FramesToRender _tmpFrames;
		App::Core::Threading::BaseThread *_readThread = nullptr;
		App::Core::Threading::BaseThread *_writeThread = nullptr;
	};
} // namespace VTX::App::Core::Player
#endif
