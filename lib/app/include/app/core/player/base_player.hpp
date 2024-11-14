#ifndef __VTX_APP_CORE_PLAYER_BASE_PLAYER__
#define __VTX_APP_CORE_PLAYER_BASE_PLAYER__

#include <memory>
#include <string>
#include <util/callback.hpp>
#include <util/collection.hpp>
#include <util/types.hpp>
#include <core/struct/frame_data_simple_prodcons.hpp>

namespace VTX::App::Core::Player
{
	class BasePlayer
	{
	  public:
		BasePlayer()							  = default;
		//devjla
		//BasePlayer( const BasePlayer & p_source ) = default;
		BasePlayer (const BasePlayer & p_source )
		{
			// FIXME
			_count	= p_source._count;
			_current = p_source._current;
			_isPlaying	   = false;
			_fps			   = p_source._fps;
			_trajectoryTimer = p_source._trajectoryTimer;
		}

		virtual ~BasePlayer() = default;

		size_t getCount() const { return _count; }
		void   setCount( const size_t p_count );

		size_t getCurrent() const;
		void   setCurrent( const size_t p_frameIndex );

		virtual void play();
		virtual void pause();
		virtual void stop();

		virtual void update( const float p_deltaTime );

		inline bool isPlaying() const { return _isPlaying; }

		virtual void reset()									= 0;
		virtual void nextFrame( const size_t p_frameCount = 1 ) = 0;

		inline uint getFPS() const { return _fps; }
		void		setFPS( const uint p_fps );

		virtual const std::string & getDisplayName() const = 0;

		virtual void StackFrame( VTX::Core::Struct::Frame elem ) = 0; // FIXME
		inline float getTrajectoryTimer() const { return _trajectoryTimer; }
		void				   setTrajectoryTimer( float p_timer ) { _trajectoryTimer = p_timer; }

		Util::Callback<>	   onPlay; 
		Util::Callback<>	   onPause;
		Util::Callback<>	   onStop;
		//Util::Callback<size_t> onFrameChange;
		Util::Callback<const VTX::Core::Struct::Frame &> onFrameChange;
		Util::Callback<uint>   onFPSChange;

	  private:
		size_t _count	= 0;
		size_t _current = 0;

		// devjla
		//bool  _isPlaying	   = false;
		std::atomic<bool> _isPlaying	   = false;
		uint  _fps			   = 2u;
		float _trajectoryTimer = 0;
	};
} // namespace VTX::App::Core::Player
#endif
