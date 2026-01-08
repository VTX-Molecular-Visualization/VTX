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
		void execute( const FilePath & p_path, const std::string * const p_buffer = nullptr );
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
} // namespace VTX::App::Action::Scene
#endif
