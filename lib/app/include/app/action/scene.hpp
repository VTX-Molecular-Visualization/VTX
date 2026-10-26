#ifndef VTX_APP_ACTION_SCENE_HPP
#define VTX_APP_ACTION_SCENE_HPP

#include "app/ecs.hpp"
#include <util/types.hpp>

namespace VTX::App::Action::Scene
{

	/**
	 * @brief Remove a system from the scene.
	 */
	struct DeleteSystem
	{
		void execute( Entity );
	};

	/**
	 * @brief Remove all selected systems.
	 */
	struct DeleteSystemSelected
	{
		void execute();
	};

	/**
	 * @brief Remove all from the scene.
	 */
	struct Clear
	{
		void execute();
	};

	/**
	 * @brief Set current color layout.
	 */
	struct SetColorLayout
	{
		void execute( Entity p_e );
	};

	/**
	 * @brief Set current graphics config.
	 */
	struct SetGraphicsConfig
	{
		void execute( Entity p_e );
	};
} // namespace VTX::App::Action::Scene
#endif
