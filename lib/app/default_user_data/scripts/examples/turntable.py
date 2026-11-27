# Python API documentation: https://vtx.drugdesign.fr/doc/python-api/

"""Orbit the camera around a structure and save one snapshot per step, to assemble into a video."""

import math

import vtx

PDB_ID = "4HHB"
FRAME_COUNT = 72
FILENAME_PREFIX = "turntable"
DISTANCE = 100.0

try:
    system = vtx.getSystem(PDB_ID)
except ValueError:
    vtx.download(PDB_ID)
    system = vtx.getSystem(PDB_ID)

center = system.aabb.center

for frame in range(FRAME_COUNT):
    angle = 2.0 * math.pi * frame / FRAME_COUNT
    vtx.camera.setPosition(
        vtx.Vec3f(center.x + DISTANCE * math.sin(angle), center.y, center.z + DISTANCE * math.cos(angle))
    )
    vtx.camera.lookAt(center)
    vtx.makeSnapshot(filename=f"{FILENAME_PREFIX}_{frame:03d}")

print(f"{FRAME_COUNT} frames saved in the snapshots folder ({FILENAME_PREFIX}_000 to {FILENAME_PREFIX}_{FRAME_COUNT - 1:03d})")
