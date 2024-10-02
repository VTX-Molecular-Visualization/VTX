#ifndef __VTX_APP_CORE_PLAYER_BASE_PLAYER__
#define __VTX_APP_CORE_PLAYER_BASE_PLAYER__

#include <memory>
#include <string>
#include <util/callback.hpp>
#include <util/collection.hpp>
#include <util/generic/base_serializable.hpp>
#include <util/types.hpp>

namespace VTX::App::Core::Player
{
	class BasePlayer : public Util::Generic::BaseSerializable
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

		virtual const std::string & getDisplayName() const = 0;

		virtual Util::JSon::Object serialize() const override
		{
			return { { "COUNT", _count }, { "CURRENT", getCurrent() }, { "FPS", _fps }, { "IS_PLAYING", _isPlaying } };
		}

		virtual void deserialize( const Util::JSon::Object & p_json ) override
		{
			setCount( p_json[ "COUNT" ].get<size_t>() );
			setCurrent( p_json[ "CURRENT" ].get<size_t>() );
			setFPS( p_json[ "FPS" ].get<uint>() );

			if ( p_json[ "IS_PLAYING" ].get<bool>() )
			{
				play();
			}
		}

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
} // namespace VTX::App::Core::Player
#endif
