#ifndef __VTX_TRAJECTORY_ENUM__
#define __VTX_TRAJECTORY_ENUM__

namespace VTX::Trajectory
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

	inline const static std::string PLAY_MODE_STRING[] = {
		"Once", "Loop", "RevertOnce", "RevertLoop", "PingPong", "Stop",
	};
} // namespace VTX::Trajectory
#endif
