#ifndef __VTX_APP_CONTROLLER_FREEFLY__
#define __VTX_APP_CONTROLLER_FREEFLY__

#include "app/controller/concepts.hpp"
#include "app/ecs.hpp"
#include "app/setting/controller.hpp"

namespace VTX::App::Controller
{

	/**
	 * @brief Freefly camera controller from inputs (free movement in 3D space).
	 */
	class Freefly : public IController
	{
	  public:
		/**
		 * @brief Called each frame.
		 */
		bool update( const float, const Setting::Controller &, Util::Math::Transform &, Vec3f & );

		/**
		 * @brief Stop movement.
		 */
		void stop() override {}
	};
} // namespace VTX::App::Controller
#endif
