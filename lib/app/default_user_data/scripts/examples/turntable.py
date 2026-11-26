# Python API documentation: https://vtx.drugdesign.fr/doc/python-api/

"""Orbit the camera around a structure and save one snapshot per step, to assemble into a video."""

import math

import vtx

PDB_ID = "4HHB"
FRAME_COUNT = 72
FILENAME_PREFIX = "turntable"

try:
    system = vtx.getSystem(PDB_ID)
except ValueError:
    vtx.download(PDB_ID)
    system = vtx.getSystem(PDB_ID)

center = system.aabb.center
start = vtx.camera.position
radius = math.hypot(start.x - center.x, start.z - center.z)
start_angle = math.atan2(start.x - center.x, start.z - center.z)
pitch = vtx.camera.rotation.euler.x

for frame in range(FRAME_COUNT):
    angle = start_angle + 2.0 * math.pi * frame / FRAME_COUNT
    vtx.camera.setPosition(
        vtx.Vec3f(center.x + radius * math.sin(angle), start.y, center.z + radius * math.cos(angle))
    )
    vtx.camera.setRotation(vtx.Quatf.fromEuler(pitch, math.degrees(angle), 0.0))
    vtx.makeSnapshot(filename=f"{FILENAME_PREFIX}_{frame:03d}")

print(f"{FRAME_COUNT} frames saved in the snapshots folder ({FILENAME_PREFIX}_000 to {FILENAME_PREFIX}_{FRAME_COUNT - 1:03d})")
