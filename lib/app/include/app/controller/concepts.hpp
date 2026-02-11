#ifndef __VTX_APP_CONTROLLER_CONCEPTS__
#define __VTX_APP_CONTROLLER_CONCEPTS__

#include <util/math/transform.hpp>

namespace VTX::App::Controller
{

	/**
	 * @brief Defines the update function.
	 */
	template<typename T>
	concept ConceptController = requires( T & p_controller, const float p_delta, Util::Math::Transform & p_transform ) {
		{ p_controller.update( p_delta, p_transform ) } -> std::same_as<void>;
	};

	/**
	 * @brief For static polymorphism.
	 */
	struct IController
	{
		virtual ~IController() = default;
	};

} // namespace VTX::App::Controller

#endif
