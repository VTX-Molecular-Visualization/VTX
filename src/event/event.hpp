#ifndef __VTX_EVENT__
#define __VTX_EVENT__

#ifdef _MSC_VER
#pragma once
#endif

#include <string>

namespace VTX
{
	namespace Event
	{
		// Global system events.
		const std::string LOG_CONSOLE = "LOG_CONSOLE";

		// Model events for notifier pattern model->views.
		enum class VTX_EVENT_MODEL : int
		{
			RENDER,
			CHANGE_REPRESENTATION
		};

	} // namespace Event
} // namespace VTX
#endif
