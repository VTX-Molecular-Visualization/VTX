#pragma once

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
	 * @brief Get frame copy.
	 */
	Frame getCurrentFrame( Entity p_entity );

	/**
	 * @brief Binder.
	 */
	class TrajectoryBinder
	{
	  public:
		void bind( Module & p_vtxModule );
	};
} // namespace VTX::App::PythonBinding
