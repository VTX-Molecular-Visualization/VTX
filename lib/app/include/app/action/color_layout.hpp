#ifndef __VTX_UI_ACTION_COLOR_LAYOUT__
#define __VTX_UI_ACTION_COLOR_LAYOUT__

#include "app/ecs.hpp"
#include <renderer/color.hpp>
#include <util/color/rgba.hpp>

namespace VTX::App::Action::ColorLayout
{
	/**
	 * @brief Set instance in the scene.
	 */
	struct SetCurrent
	{
		void execute( const ECS::Entity );
	};

	/**
	 * @brief Change a single color.
	 */
	struct Change
	{
		void execute( const ECS::Entity, const Index, const Util::Color::Rgba & );
	};

	/**
	 * @brief Change all colors.
	 */
	struct ChangeAll
	{
		void execute( const ECS::Entity, const Renderer::Color::LayoutArray & );
	};

	/**
	 * @brief Randomize all colors.
	 */
	struct Randomize
	{
		void execute( const ECS::Entity );
	};

} // namespace VTX::App::Action::ColorLayout
#endif
