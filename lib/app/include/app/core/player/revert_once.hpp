#ifndef __VTX_APP_CORE_PLAYER_REVERT_ONCE__
#define __VTX_APP_CORE_PLAYER_REVERT_ONCE__

#include "app/core/player/base_player.hpp"
#include "app/core/player/players.hpp"

namespace VTX::App::Core::Player
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

		void StackFrame( Frame elem ) override {}; // FIXME
	};
} // namespace VTX::App::Core::Player
#endif
