#ifndef __VTX_UI_ACTION_COLOR_LAYOUT__
#define __VTX_UI_ACTION_COLOR_LAYOUT__

#include "app/action/library.hpp"
#include "app/library/preset/color_layout.hpp"
#include <util/color/rgba.hpp>

namespace VTX::App::Action::ColorLayout
{
	struct SetCurrent
	{
		void execute( const std::string_view );
	};

	struct Change
	{
		void execute( const std::string_view, const Index, const Util::Color::Rgba & );
	};

	struct ChangeAll
	{
		void execute( const std::string_view, const Renderer::Color::LayoutArray & p_colors );
	};

	struct Randomize
	{
		void execute( const std::string_view );
	};

} // namespace VTX::App::Action::ColorLayout
#endif
