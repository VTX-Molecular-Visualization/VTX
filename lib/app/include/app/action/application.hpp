#ifndef __VTX_APP_ACTION_APPLICATION__
#define __VTX_APP_ACTION_APPLICATION__

#include <util/types.hpp>
#include <vector>

namespace VTX::App::Action::Application
{

	struct Quit
	{
		void execute();
	};

	struct Resize
	{
		void execute( const size_t p_width, const size_t p_height );
	};

} // namespace VTX::App::Action::Application
#endif
