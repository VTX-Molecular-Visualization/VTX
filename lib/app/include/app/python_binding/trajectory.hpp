#ifndef __VTX_APP_PYTHON_BINDING_TRAJECTORY__
#define __VTX_APP_PYTHON_BINDING_TRAJECTORY__

#include "app/ecs.hpp"
#include "app/python_binding/module.hpp"
#include <core/struct/trajectory.hpp>
#include <cstddef>
#include <util/constants.hpp>

namespace VTX::App::PythonBinding
{
	/**
	 * @brief Frame API.
	 */
	struct Frame
	{
		std::size_t			index = INVALID_INDEX;
		Core::Struct::Frame positions;
	};

	/**
	 * @brief Trajectory API.
	 */
	struct Trajectory
	{
		Entity entity = InvalidEntity;
	};

	/**
	 * @brief Binder.
	 */
	class TrajectoryBinder
	{
	  public:
		void bind( Module & p_vtxModule );
	};
} // namespace VTX::App::PythonBinding

#endif
