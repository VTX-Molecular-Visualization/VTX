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
		// Global system events [n-n].
		using VTX_EVENT = std::string;

		namespace Global
		{
			const VTX_EVENT LOG_CONSOLE			= "LOG_CONSOLE";
			const VTX_EVENT UPDATE_PROGRESS_BAR = "UPDATE_PROGRESS_BAR";
		} // namespace Global

		// Model events for notifier pattern (model->views)[1-n].
		enum class VTX_EVENT_MODEL : int
		{
			RENDER,
			CHANGE_REPRESENTATION
		};

	} // namespace Event
} // namespace VTX
#endif
