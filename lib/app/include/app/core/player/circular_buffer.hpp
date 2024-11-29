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

		void play() override;
		void pause() override;
		void stop() override;

		void stackFrame( VTX::Core::Struct::Frame elem )
		{
			_tmpFrames.addElement( elem );
		} // FIXME taking a copy as input?
		bool getRefFrame( VTX::Core::Struct::Frame & frame ) { return _tmpFrames.readElement( frame ); }
		void removeRefFrame( VTX::Core::Struct::Frame & elem ) { _tmpFrames.removeElement( elem ); }
		bool getCopyFrame( VTX::Core::Struct::Frame & frame ) { return _tmpFrames.getCopyFrame( frame ); }

		// FIXME test to update progress slider in UI
		const size_t getIndex( void ) const { return _tmpFrames.getindex(); }

	  private:
		// devjla
		VTX::Core::Struct::FramesToRender  _tmpFrames;
		App::Core::Threading::BaseThread * _readThread	= nullptr;
		App::Core::Threading::BaseThread * _writeThread = nullptr;
	};
} // namespace VTX::App::Core::Player
#endif
