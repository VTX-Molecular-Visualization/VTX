#ifndef __VTX_APP_CORE_PLAYER_STOP__
#define __VTX_APP_CORE_PLAYER_STOP__

#include "app/core/player/base_player.hpp"

namespace VTX::App::Core::Player
{
	class Stop : public BasePlayer
	{
	  public:
		inline static const std::string DISPLAYED_NAME = "Stop";

	  public:
		Stop();
		Stop( const Stop & p_source );

		void play() override;

		const std::string & getDisplayName() const override { return DISPLAYED_NAME; }

		void reset() override;
		void nextFrame( const size_t frameCount = 1 ) override;

		void stackFrame( Frame elem ) override {}; // FIXME
	};
} // namespace VTX::App::Core::Player
#endif
