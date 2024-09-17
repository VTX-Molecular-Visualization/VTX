#ifndef __VTX_APP_CORE_PLAYER_PLAYERS__
#define __VTX_APP_CORE_PLAYER_PLAYERS__

#include "app/core/player/base_player.hpp"
#include "util/collection.hpp"

namespace VTX::App::Core::Player
{
	using Players = Util::Collection<std::unique_ptr<BasePlayer>>;
} // namespace VTX::App::Core::Player
#endif
