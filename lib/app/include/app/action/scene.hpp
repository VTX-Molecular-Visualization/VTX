#ifndef __VTX_APP_ACTION_SCENE__
#define __VTX_APP_ACTION_SCENE__

#include "app/ecs.hpp"
#include <string_view>
#include <util/types.hpp>

namespace VTX::App::Action::Scene
{

	/**
	 * @brief Remove a system from the scene.
	 */
	struct DeleteSystem
	{
		void execute( const Entity );
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
		void execute( const Entity p_e );
		void execute( const std::string_view p_name );
	};

	/**
	 * @brief Set current graphics config.
	 */
	struct SetGraphicsConfig
	{
		void execute( const Entity p_e );
		void execute( const std::string_view p_name );
	};
} // namespace VTX::App::Action::Scene
#endif
