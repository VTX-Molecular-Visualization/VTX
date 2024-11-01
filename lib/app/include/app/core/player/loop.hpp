#ifndef __VTX_APP_CORE_PLAYER_LOOP__
#define __VTX_APP_CORE_PLAYER_LOOP__

#include "app/core/player/base_player.hpp"
#include "app/core/player/players.hpp"

namespace VTX::App::Core::Player
{
	class Loop : public BasePlayer
	{
	  public:
		inline static const std::string DISPLAYED_NAME = "Loop";

	  public:
		Loop()						  = default;
		Loop( const Loop & p_source ) = default;

		const std::string & getDisplayName() const override { return DISPLAYED_NAME; }

		void reset() override;
		void nextFrame( const size_t frameCount = 1 ) override;

		void StackFrame( VTX::Core::Struct::Frame elem ) override {}; // FIXME
	};
} // namespace VTX::App::Core::Player
#endif
