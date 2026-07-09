def require(condition, message):
    if not condition:
        raise RuntimeError(message)


vec2 = vtx.Vec2f(1.0, 2.0)
vec3 = vtx.Vec3f(1.0, 2.0, 3.0)
vec4 = vtx.Vec4f(1.0, 2.0, 3.0, 4.0)
require(vec2.x == 1.0 and vec2.y == 2.0, "Unexpected Vec2f")
require(vec3.x == 1.0 and vec3.y == 2.0 and vec3.z == 3.0, "Unexpected Vec3f")
require(
    vec4.x == 1.0 and vec4.y == 2.0 and vec4.z == 3.0 and vec4.w == 4.0,
    "Unexpected Vec4f",
)

transform = vtx.Transform()
transform.position = vec3
transform.rotation = vtx.Quatf(0.0, 0.0, 0.0)
transform.scale = vtx.Vec3f(2.0, 2.0, 2.0)
require(
    transform.position.z == 3.0
    and transform.rotation.w == 1.0
    and transform.scale.x == 2.0,
    "Unexpected transform",
)

resolution = vtx.Resolution("HD", 1280, 720)
require(
    resolution.name == "HD"
    and resolution.width == 1280
    and resolution.height == 720,
    "Unexpected resolution",
)

vtx.setColorLayout("jmol")
vtx.setGraphicsConfig("default")

color = vtx.Rgba(0.1, 0.2, 0.3)
color.a = 0.4
require(
    abs(color.r - 0.1) < 1e-6
    and abs(color.g - 0.2) < 1e-6
    and abs(color.b - 0.3) < 1e-6
    and abs(color.a - 0.4) < 1e-6,
    "Unexpected RGBA",
)

index_range = vtx.Range(2, 5)
single_index_range = vtx.Range(7)
require(
    index_range.first == 2
    and index_range.last == 5
    and index_range.count == 3
    and single_index_range.count == 1,
    "Unexpected range",
)

index_ranges = vtx.RangeList([vtx.Range(0, 2), vtx.Range(4, 6)])
value_ranges = vtx.RangeList([0, 1, 4, 5])
require(
    index_ranges.rangeCount == 2
    and index_ranges.count == 4
    and index_ranges.ranges[1].first == 4
    and value_ranges.count == 4,
    "Unexpected range list",
)

bounding_box = vtx.AABB(vtx.Vec3f(0.0, 0.0, 0.0), vtx.Vec3f(2.0, 2.0, 2.0))
point_box = vtx.AABB(vtx.Vec3f(1.0, 2.0, 3.0))
sphere_box = vtx.AABB(vtx.Vec3f(0.0, 0.0, 0.0), 2.0)
require(
    bounding_box.valid
    and bounding_box.min.x == 0.0
    and bounding_box.max.x == 2.0
    and bounding_box.center.x == 1.0
    and point_box.radius == 0.0
    and sphere_box.radius > 0.0,
    "Unexpected AABB",
)

for function_name in (
    "newScene",
    "clear",
    "deleteSystem",
    "deleteSystemSelected",
    "makeSnapshot",
    "associateTrajectory",
    "download",
):
    require(callable(getattr(vtx, function_name)), f"Missing binding: {function_name}")

position = vtx.Vec3f(1.0, 2.0, 3.0)
rotation = vtx.Quatf(0.0, 0.0, 0.0)

vtx.setCameraPosition(position)
vtx.setCameraRotation(rotation)
vtx.setCameraFov(45.0)
vtx.setCameraNearClip(0.1)
vtx.setCameraFarClip(1000.0)
vtx.setCameraProjectionOrthographic()
vtx.setCameraProjectionPerspective()

camera_position = vtx.getCameraPosition()
camera_rotation = vtx.getCameraRotation()
require(camera_position.x == position.x, "Unexpected camera position x")
require(camera_position.y == position.y, "Unexpected camera position y")
require(camera_position.z == position.z, "Unexpected camera position z")
require(camera_rotation.w == rotation.w, "Unexpected camera rotation w")

try:
    from pathlib import Path

    system_path = str(Path(__file__).with_name("1AGA.mmtf"))
except NameError:
    system_path = "data/1AGA.mmtf"

vtx.openFile(system_path)
system = vtx.getSystemIdByPdb("1aga")
require(system == vtx.getSystemIdByFileName("1aga.MMTF"), "File name lookup should ignore case")
require(
    system
    == vtx.getSystemIdByName(
        "the agarose double helix and its function in agarose gel structure"
    ),
    "System name lookup should ignore case",
)

vtx.selectAll()
require(vtx.getSelectionState(system) == vtx.SELECTION_STATE.FULL, "System should be selected")
vtx.clearSelection(system)
require(vtx.getSelectionState(system) == vtx.SELECTION_STATE.NONE, "System selection should be cleared")

vtx.select(system, vtx.SYSTEM_ITEM.RESIDUE, [0, 1, 2], True, False)
require(
    vtx.getSelectionState(system, vtx.SYSTEM_ITEM.RESIDUE, 0) == vtx.SELECTION_STATE.FULL,
    "Residue 0 should be selected",
)
require(
    vtx.getSelectionState(system, vtx.SYSTEM_ITEM.RESIDUE, 3) == vtx.SELECTION_STATE.NONE,
    "Residue 3 should not be selected",
)

vtx.setColorScheme(system, vtx.COLOR_SCHEME.CHAIN)
vtx.setColorScheme(
    system,
    vtx.COLOR_SCHEME.RESIDUE,
    vtx.SYSTEM_ITEM.RESIDUE,
    vtx.RangeList(vtx.Range(0, 2)),
)
vtx.setColorSchemeSelected(vtx.COLOR_SCHEME.CUSTOM, 228)
vtx.setColorSchemeSelected(vtx.COLOR_SCHEME.RESIDUE)
vtx.setSecondaryStructureColorScheme(
    system, vtx.SECONDARY_STRUCTURE_COLOR_SCHEME.STRUCTURE
)
vtx.setRepresentation(system, "sticks and ribbons")
vtx.setRepresentation(
    system,
    "sticks",
    vtx.SYSTEM_ITEM.RESIDUE,
    vtx.RangeList(vtx.Range(0, 2)),
)
vtx.setRepresentationSelected("sticks")

vtx.setVisibleSelected(False)
require(
    vtx.getVisibleState(system, vtx.SYSTEM_ITEM.RESIDUE, 0) == vtx.VISIBLE_STATE.HIDDEN,
    "Selected residue should be hidden",
)
require(
    vtx.getVisibleState(system, vtx.SYSTEM_ITEM.RESIDUE, 3) == vtx.VISIBLE_STATE.VISIBLE,
    "Unselected residue should remain visible",
)

vtx.setVisible(system, vtx.SYSTEM_ITEM.RESIDUE, [0, 1, 2], True)
require(
    vtx.getVisibleState(system, vtx.SYSTEM_ITEM.RESIDUE, 0) == vtx.VISIBLE_STATE.VISIBLE,
    "Residue should be visible",
)

vtx.hideAll()
require(vtx.getVisibleState(system) == vtx.VISIBLE_STATE.HIDDEN, "System should be hidden")
vtx.showAll()
require(vtx.getVisibleState(system) == vtx.VISIBLE_STATE.VISIBLE, "System should be visible")

vtx.orientCamera()
vtx.straightTravelCamera(position, rotation, 500.0)
vtx.resetCamera()

vtx.clearSelection()
require(vtx.getSelectionState(system) == vtx.SELECTION_STATE.NONE, "Selection should be cleared")

vtx.quit()

print("Python binding smoke test completed")
