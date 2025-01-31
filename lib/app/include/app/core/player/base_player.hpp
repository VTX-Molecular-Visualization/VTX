#ifndef __VTX_APP_CORE_PLAYER_BASE_PLAYER__
#define __VTX_APP_CORE_PLAYER_BASE_PLAYER__

#include <memory>
#include <string>
#include <util/callback.hpp>
#include <util/collection.hpp>
#include <util/types.hpp>

namespace VTX::App::Core::Player
{
	using Frame = std::vector<Vec3f>;
	class BasePlayer
	{
	  public:
		BasePlayer() = default;
		/**
		 * @brief Copy ctor explicit definition because implicit definition does not exists due to the _isPlaying
		 * atomic. member
		 */
		BasePlayer( const BasePlayer & p_source )
		{
			_count			 = p_source._count;
			_current		 = p_source._current;
			_isPlaying		 = false;
			_fps			 = p_source._fps;
			_trajectoryTimer = p_source._trajectoryTimer;
		}

		virtual ~BasePlayer() = default;

		size_t getCount() const { return _count; }
		/**
		 * @brief Sets the current frame count and updates the current frame to be played.
		 */
		void setCount( const size_t p_count );

		/**
		 * @brief Updates the internal counter of the frame to be played.
		 */
		size_t getCurrent() const;
		/**
		 * @brief Updates the current frame to be played.
		 */
		void setCurrent( const size_t p_frameIndex );

		virtual void play();
		virtual void pause();
		virtual void stop();

		/**
		 * @brief Callback to decide whether a new frame needs to be played
		 */
		void update( const float p_deltaTime, const float p_elaspedTime );

		inline bool isPlaying() const { return _isPlaying; }

		virtual void reset()									= 0;
		virtual void nextFrame( const size_t p_frameCount = 1 ) = 0;

		inline uint getFPS() const { return _fps; }
		void		setFPS( const uint p_fps );

		virtual const std::string & getDisplayName() const = 0;

		/**
		 * @brief Virtual method only implemented in the circular buffer player
		 */
		virtual void stackFrame( Frame elem ) = 0;
		inline float getTrajectoryTimer() const { return _trajectoryTimer; }
		void		 setTrajectoryTimer( float p_timer ) { _trajectoryTimer = p_timer; }

		Util::Callback<>			  onPlay;
		Util::Callback<>			  onPause;
		Util::Callback<>			  onStop;
		Util::Callback<const Frame &> onFrameChange;
		Util::Callback<uint>		  onFPSChange;

	  private:
		size_t			  _count		   = 0;
		size_t			  _current		   = 0;
		std::atomic<bool> _isPlaying	   = false;
		uint			  _fps			   = 1u;
		float			  _trajectoryTimer = 0.f;
	};
} // namespace VTX::App::Core::Player
#endif
