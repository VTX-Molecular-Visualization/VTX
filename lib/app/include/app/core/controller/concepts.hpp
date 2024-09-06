#ifndef __VTX_APP_CORE_CONTROLLER_CONCEPT__
#define __VTX_APP_CORE_CONTROLLER_CONCEPT__

#include <concepts>

template<typename C>
concept Concept = requires( C p_controller, const float p_deltaTime ) {
	{ p_controller.init() } -> std::same_as<void>;
	{ p_controller.update( p_deltaTime ) } -> std::same_as<void>;
};

#endif
