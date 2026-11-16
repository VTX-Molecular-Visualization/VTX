def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def almost_equal(left, right, tolerance=1e-6):
    return abs(left - right) <= tolerance


try:
    from pathlib import Path

    trajectory_path = str(Path(__file__).with_name("1gcn_traj.xtc"))
    other_system_path = str(Path(__file__).with_name("1AGA.mmtf"))
except NameError:
    trajectory_path = "data/1gcn_traj.xtc"
    other_system_path = "data/1AGA.mmtf"

trajectory_system = vtx.getSystem("1gcn")
trajectory_system.associateTrajectory(trajectory_path)
trajectory = trajectory_system.trajectory
require(trajectory.frameCount > 1, "Unexpected trajectory frame count")
require(trajectory.loadedFrameCount >= 1, "No trajectory frame is available")
require(trajectory.availableFrames.first == 0, "Unexpected available frame range")
first_frame = trajectory.getFrame(0)
require(
    len(first_frame) == trajectory_system.atomCount,
    "Trajectory frame atom count does not match its system",
)
require(len(trajectory) == trajectory.frameCount, "Unexpected trajectory length")
require(
    len(trajectory[-1]) == trajectory_system.atomCount,
    "Last trajectory frame atom count does not match its system",
)

trajectory.setPlayMode(vtx.TRAJECTORY_PLAY_MODE.FORWARD_LOOP)
require(
    trajectory.playMode == vtx.TRAJECTORY_PLAY_MODE.FORWARD_LOOP,
    "Unexpected trajectory play mode",
)
trajectory.setSpeed(42.0)
require(almost_equal(trajectory.speed, 42.0), "Unexpected trajectory speed")
try:
    trajectory.setSpeed(float("nan"))
except ValueError:
    pass
else:
    raise AssertionError("A non-finite trajectory speed should raise ValueError")
try:
    trajectory.jumpTo(trajectory.frameCount)
except IndexError:
    pass
else:
    raise AssertionError("Jumping outside of the trajectory should raise IndexError")
trajectory.play()
require(not trajectory.paused, "Trajectory should be playing")
trajectory.pause()
require(trajectory.paused, "Trajectory should be paused")
trajectory.stop()
require(
    trajectory.paused and trajectory.requestedFrameIndex == 0,
    "Trajectory should be stopped at its first frame",
)

require(
    trajectory_system.trajectory == trajectory_system.trajectory
    and len({trajectory_system.trajectory, trajectory_system.trajectory}) == 1,
    "Unexpected trajectory handle equality",
)
vtx.openFile(other_system_path)
other_system = vtx.getSystem("1aga")
try:
    other_system.associateTrajectory(trajectory_path)
except RuntimeError as error:
    require("different atom count" in str(error), f"Unexpected association error: {error}")
else:
    raise AssertionError("Associating a trajectory with a different atom count should raise RuntimeError")
try:
    trajectory_system.associateTrajectory(trajectory_path + ".missing")
except Exception:
    pass
else:
    raise AssertionError("Associating a missing trajectory file should raise")
require(
    trajectory_system.trajectory.frameCount == trajectory.frameCount,
    "A failed association should keep the current trajectory",
)
other_system.delete()

trajectory_system.delete()
require(len(first_frame) > 0, "Copied trajectory frame became unavailable")

print("Python trajectory binding smoke test completed")
