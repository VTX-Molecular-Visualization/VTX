def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def almost_equal(left, right, tolerance=1e-6):
    return abs(left - right) <= tolerance


try:
    from pathlib import Path

    trajectory_path = str(Path(__file__).with_name("1gcn_traj.xtc"))
except NameError:
    trajectory_path = "data/1gcn_traj.xtc"

trajectory_system = vtx.getSystem("1gcn")
trajectory_system.associateTrajectory(trajectory_path)
trajectory = trajectory_system.getTrajectory()
require(trajectory.isMultiFrame(), "Expected a multi-frame trajectory")
require(trajectory.frameCount > 1, "Unexpected trajectory frame count")
require(trajectory.loadedFrameCount >= 1, "No trajectory frame is available")
require(trajectory.availableFrames.first == 0, "Unexpected available frame range")
require(
    len(trajectory.getFrame(0)) == trajectory_system.atomCount,
    "Trajectory frame atom count does not match its system",
)

trajectory.setPlayMode(vtx.TRAJECTORY_PLAY_MODE.FORWARD_LOOP)
require(
    trajectory.playMode == vtx.TRAJECTORY_PLAY_MODE.FORWARD_LOOP,
    "Unexpected trajectory play mode",
)
trajectory.setSpeed(42.0)
require(almost_equal(trajectory.speed, 42.0), "Unexpected trajectory speed")
trajectory.play()
require(not trajectory.paused, "Trajectory should be playing")
trajectory.pause()
require(trajectory.paused, "Trajectory should be paused")
trajectory.stop()
require(
    trajectory.paused and trajectory.requestedFrameIndex == 0,
    "Trajectory should be stopped at its first frame",
)

vtx.deleteSystem(trajectory_system.id)

print("Python trajectory binding smoke test completed")
