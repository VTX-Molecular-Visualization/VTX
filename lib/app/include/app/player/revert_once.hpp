#ifndef __VTX_APP_PLAYER_REVERT_ONCE__
#define __VTX_APP_PLAYER_REVERT_ONCE__

#include "app/player/base_player.hpp"


namespace VTX::App::Player
{
	class RevertOnce : public BasePlayer
	{
	  public:
		inline static const std::string DISPLAYED_NAME = "Revert Once";

	  public:
		RevertOnce()							  = default;
		RevertOnce( const RevertOnce & p_source ) = default;

		const std::string & getDisplayName() const override { return DISPLAYED_NAME; }

		void reset() override;
		void nextFrame( const size_t frameCount = 1 ) override;

		void stackFrame( Frame elem ) override {}; // FIXME
	};
} // namespace VTX::App::Player
#endif
