#ifndef __VTX_APP_CORE_MODE_CONCEPTS__
#define __VTX_APP_CORE_MODE_CONCEPTS__

#include "base_mode.hpp"
#include <concepts>

namespace VTX::App::Core::Mode
{

	template<typename M>
	concept ConceptMode = std::is_base_of<BaseMode, M>::value
						  && requires( M p_mode, const float p_deltaTime, const float p_elapsedTime ) {
								 { p_mode.update( p_deltaTime, p_elapsedTime ) } -> std::same_as<void>;
							 };

} // namespace VTX::App::Core::Mode
#endif
