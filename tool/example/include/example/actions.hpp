#ifndef __VTX_TOOL_EXAMPLE_ACTIONS_
#define __VTX_TOOL_EXAMPLE_ACTIONS_

#include <app/ui/concepts.hpp>

namespace VTX::Tool::Example::Action
{

	struct New : public App::UI::DescAction
	{
		New()
		{
			name	 = "New";
			tip		 = "Create a new project";
			icon	 = ":/sprite/file/new.png";
			shortcut = "Ctrl+N";
			// trigger	 = []() { App::VTX_ACTION().execute<App::Action::Application::NewScene>(); };
		}
	};

} // namespace VTX::Tool::Example::Action

#endif
