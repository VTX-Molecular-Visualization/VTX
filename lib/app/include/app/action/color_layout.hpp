#ifndef __VTX_UI_ACTION_COLOR_LAYOUT__
#define __VTX_UI_ACTION_COLOR_LAYOUT__

#include "app/ecs.hpp"
#include <renderer/color.hpp>
#include <util/color/rgba.hpp>

namespace VTX::App::Action::ColorLayout
{

	/**
	 * @brief Change a single color.
	 */
	struct Change
	{
		void execute( Entity, Index, const Util::Color::Rgba & );
	};

	/**
	 * @brief Change all colors.
	 */
	struct ChangeAll
	{
		void execute( Entity, const Renderer::Color::LayoutArray & );
	};

	/**
	 * @brief Randomize all colors.
	 */
	struct Randomize
	{
		void execute( Entity );
	};

} // namespace VTX::App::Action::ColorLayout
#endif
