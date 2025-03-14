#ifndef __VTX_APP_CORE_PLAYER_CONCEPTS__
#define __VTX_APP_CORE_PLAYER_CONCEPTS__

#include "base_player.hpp"
#include <concepts>

namespace VTX::App::Core::Player
{

	template<typename P>
	concept ConceptPlayer = std::is_base_of<BasePlayer, P>::value
							&& requires( P p_player, const float p_deltaTime, const float p_elapsedTime ) {
								   { p_player.update( p_deltaTime, p_elapsedTime ) } -> std::same_as<void>;
							   };

} // namespace VTX::App::Core::Player
#endif
