#ifndef __VTX_APP_MODEL_CHEMISTRY_ENUM_TRAJECTORY__
#define __VTX_APP_MODEL_CHEMISTRY_ENUM_TRAJECTORY__

#include <string>
#include <vector>

namespace VTX::App::Component::Chemistry
{
	enum class PlayMode
	{
		Once,
		Loop,
		RevertOnce,
		RevertLoop,
		PingPong,
		Stop,

		COUNT
	};

	inline const std::vector<std::string> PLAY_MODE_STRING = {
		"Once", "Loop", "RevertOnce", "RevertLoop", "PingPong", "Stop",
	};
} // namespace VTX::App::Component::Chemistry
#endif
