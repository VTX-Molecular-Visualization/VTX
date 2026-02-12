#ifndef __VTX_APP_CONTROLLER_CONCEPTS__
#define __VTX_APP_CONTROLLER_CONCEPTS__

#include <util/math/transform.hpp>

namespace VTX::App::Controller
{

	/**
	 * @brief Defines the update function.
	 */
	template<typename T>
	concept ConceptController
		= requires( T & p_controller, const float p_delta, Util::Math::Transform & p_transform, Vec3f & p_target ) {
			  { p_controller.update( p_delta, p_transform, p_target ) } -> std::same_as<bool>;
		  };

	/**
	 * @brief For static polymorphism.
	 */
	struct IController
	{
		virtual ~IController() = default;
		virtual void stop()	   = 0;
	};

} // namespace VTX::App::Controller

#endif
