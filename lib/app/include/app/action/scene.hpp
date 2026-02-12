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
		LoadSystem();
		void execute( FilePath p_path );
		void execute( FilePath p_path, std::string && p_buffer );
		void wait() noexcept;
		struct _Data;
		struct Del
		{
			void operator()( _Data * ) noexcept;
		};
		std::unique_ptr<_Data, Del> _data = nullptr;
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
