#ifndef __VTX_APP_CORE_PLAYER_REVERT_LOOP__
#define __VTX_APP_CORE_PLAYER_REVERT_LOOP__

#include "app/core/player/base_player.hpp"

namespace VTX::App::Core::Player
{
	class RevertLoop : public BasePlayer
	{
	  public:
		inline static const std::string DISPLAYED_NAME = "Revert Loop";

	  public:
		RevertLoop()							  = default;
		RevertLoop( const RevertLoop & p_source ) = default;

		const std::string & getDisplayName() const override { return DISPLAYED_NAME; }

		void reset() override;
		void nextFrame( const size_t frameCount = 1 ) override;

		void stackFrame( Frame elem ) override {}; // FIXME
	};
} // namespace VTX::App::Core::Player
#endif
