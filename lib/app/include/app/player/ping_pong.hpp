#ifndef __VTX_APP_PLAYER_PING_PONG__
#define __VTX_APP_PLAYER_PING_PONG__

#include "app/player/base_player.hpp"

namespace VTX::App::Player
{
	class PingPong : public BasePlayer
	{
	  public:
		inline static const std::string DISPLAYED_NAME = "Ping Pong";

	  public:
		PingPong()							  = default;
		PingPong( const PingPong & p_source ) = default;

		const std::string & getDisplayName() const override { return DISPLAYED_NAME; }

		void reset() override;
		void nextFrame( const size_t frameCount = 1 ) override;

		bool isPlayingForward() const { return _forward; }
		void setPlayingForward( const bool p_forward ) { _forward = p_forward; }

		void stackFrame( Frame elem ) override {}; // FIXME

	  private:
		bool _forward = true;
	};
} // namespace VTX::App::Player
#endif
