#ifndef __VTX_APP_ACTION_SCENE__
#define __VTX_APP_ACTION_SCENE__

#include "app/ecs.hpp"
#include <util/types.hpp>

namespace VTX::App::Action::Scene
{

	/**
	 * @brief Load a molecular system from disk or buffer.
	 */
	struct LoadSystem
	{
		void execute( FilePath p_path );
		void execute( FilePath p_path, std::string && p_buffer );
	};

	/**
	 * @brief Load a molecular system from disk or buffer.
	 */
	struct DeleteSystem
	{
		void execute( const ECS::Entity );
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
		void execute( const ECS::Entity p_e );
	};

	/**
	 * @brief Set current graphics config.
	 */
	struct SetGraphicsConfig
	{
		void execute( const ECS::Entity p_e );
	};
} // namespace VTX::App::Action::Scene
#endif
