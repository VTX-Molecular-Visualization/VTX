#ifndef __VTX_APP_CORE_CONTROLLER_CONCEPTS__
#define __VTX_APP_CORE_CONTROLLER_CONCEPTS__

#include "base_controller.hpp"
#include <concepts>

namespace VTX::App::Core::Controller
{
	template<typename C>
	concept ConceptController
		= std::is_base_of<BaseController, C>::value
		  && requires( C p_mode, const float p_deltaTime, const float p_elapsedTime, const bool p_active ) {
				 { p_mode.update( p_deltaTime, p_elapsedTime ) } -> std::same_as<void>;
			 };

} // namespace VTX::App::Core::Controller

#endif
