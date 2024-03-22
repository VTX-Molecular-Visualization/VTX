#ifndef __VTX_APP_CORE_TRAJECTORY_PLAYER_BASE_PLAYER__
#define __VTX_APP_CORE_TRAJECTORY_PLAYER_BASE_PLAYER__

#include "app/component/chemistry/_fwd.hpp"
#include "app/core/collection.hpp"
#include <memory>
#include <string>
#include <util/callback.hpp>
#include <util/types.hpp>

namespace VTX::App::Core::TrajectoryPlayer
{
	class BasePlayer
	{
	  public:
		BasePlayer()							  = default;
		BasePlayer( const BasePlayer & p_source ) = default;

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

		virtual std::unique_ptr<BasePlayer> clone() const			 = 0;
		virtual const std::string &			getDisplayName() const	 = 0;
		virtual const CollectionKey &		getCollectionKey() const = 0;

		Util::Callback<>	   onPlay;
		Util::Callback<>	   onPause;
		Util::Callback<>	   onStop;
		Util::Callback<size_t> onFrameChange;
		Util::Callback<uint>   onFPSChange;

	  private:
		size_t _count	= 0;
		size_t _current = 0;

		bool  _isPlaying	   = true;
		uint  _fps			   = 1u;
		float _trajectoryTimer = 0;
	};
} // namespace VTX::App::Core::TrajectoryPlayer
#endif
