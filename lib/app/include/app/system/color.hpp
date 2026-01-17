#ifndef __VTX_APP_SYSTEM_COLOR__
#define __VTX_APP_SYSTEM_COLOR__

#include <core/struct/system.hpp>
#include <renderer/color.hpp>

namespace VTX::App::System
{

	/**
	 * @brief Store atom color index in the current layout.
	 */
	struct Color
	{
		/**
		 * @brief Atom colors.
		 */
		std::vector<Renderer::Color::ColorIndex> atoms;
	};

} // namespace VTX::App::System

#endif
