#ifndef __VTX_UI_ACTIONS__
#define __VTX_UI_ACTIONS__

#include <app/core/input/keys.hpp>
#include <string>

namespace VTX::UI
{

	struct Action
	{
		std::string				   name;
		std::optional<std::string> tip	= std::nullopt;
		std::optional<std::string> icon = std::nullopt;
		// callback or vtx action
		// std::optional<App::Core::Input::Shortcut> shortcut;
	};

} // namespace VTX::UI

#endif
