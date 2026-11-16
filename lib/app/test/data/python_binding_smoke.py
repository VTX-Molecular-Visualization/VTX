def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def almost_equal(left, right, tolerance=1e-6):
    return abs(left - right) <= tolerance


def rgba_equal(left, right):
    return (
        almost_equal(left.r, right.r)
        and almost_equal(left.g, right.g)
        and almost_equal(left.b, right.b)
        and almost_equal(left.a, right.a)
    )


def set_and_require_properties(value, properties, label):
    for property_name, expected in properties:
        setattr(value, property_name, expected)
        actual = getattr(value, property_name)
        matches = (
            almost_equal(actual, expected)
            if isinstance(expected, float)
            else actual == expected
        )
        require(matches, f"Unexpected {label} property: {property_name}")


vec2 = vtx.Vec2f(1.0, 2.0)
vec3 = vtx.Vec3f(1.0, 2.0, 3.0)
vec4 = vtx.Vec4f(1.0, 2.0, 3.0, 4.0)
require(almost_equal(vec2.x, 1.0) and almost_equal(vec2.y, 2.0), "Unexpected Vec2f")
require(
    almost_equal(vec3.x, 1.0)
    and almost_equal(vec3.y, 2.0)
    and almost_equal(vec3.z, 3.0),
    "Unexpected Vec3f",
)
require(
    almost_equal(vec4.x, 1.0)
    and almost_equal(vec4.y, 2.0)
    and almost_equal(vec4.z, 3.0)
    and almost_equal(vec4.w, 4.0),
    "Unexpected Vec4f",
)

identity = vtx.Quatf()
require(
    almost_equal(identity.w, 1.0)
    and almost_equal(identity.x, 0.0)
    and almost_equal(identity.y, 0.0)
    and almost_equal(identity.z, 0.0),
    "Unexpected default quaternion",
)

explicit_quat = vtx.Quatf(0.5, 0.5, 0.5, 0.5)
require(
    almost_equal(explicit_quat.w, 0.5) and almost_equal(explicit_quat.x, 0.5),
    "Unexpected explicit quaternion",
)

half_sqrt2 = 0.5 ** 0.5
yaw_quat = vtx.Quatf.fromEuler(0.0, 90.0, 0.0)
require(
    almost_equal(yaw_quat.w, half_sqrt2, 1e-4)
    and almost_equal(yaw_quat.x, 0.0, 1e-4)
    and almost_equal(yaw_quat.y, half_sqrt2, 1e-4)
    and almost_equal(yaw_quat.z, 0.0, 1e-4),
    "Unexpected quaternion from Euler angles",
)

pitch_euler = vtx.Quatf.fromEuler(30.0, 0.0, 0.0).euler
require(
    almost_equal(pitch_euler.x, 30.0, 1e-3)
    and almost_equal(pitch_euler.y, 0.0, 1e-3)
    and almost_equal(pitch_euler.z, 0.0, 1e-3),
    "Unexpected Euler angles from quaternion",
)

vtx.getColorLayoutPreset("jmol").apply()
vtx.getGraphicsConfigPreset("default").apply()

color = vtx.Rgba(0.1, 0.2, 0.3)
color.a = 0.4
require(
    almost_equal(color.r, 0.1)
    and almost_equal(color.g, 0.2)
    and almost_equal(color.b, 0.3)
    and almost_equal(color.a, 0.4),
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
    and almost_equal(bounding_box.min.x, 0.0)
    and almost_equal(bounding_box.max.x, 2.0)
    and almost_equal(bounding_box.center.x, 1.0)
    and almost_equal(point_box.radius, 0.0)
    and sphere_box.radius > 0.0,
    "Unexpected AABB",
)

for function_name in (
    "newScene",
    "deleteSystemSelected",
    "makeSnapshot",
    "openFile",
    "download",
    "getSystem",
    "getSystems",
    "setResolution",
    "getResolution",
):
    require(callable(getattr(vtx, function_name)), f"Missing binding: {function_name}")

try:
    vtx.download("../invalid")
except ValueError:
    pass
else:
    raise AssertionError("An invalid system id should raise ValueError")

for removed_name in (
    "setCameraFov",
    "orientCamera",
    "getSystemIdByName",
    "associateTrajectory",
    "deleteSystem",
    "select",
    "getSelectionState",
    "setVisible",
    "getVisibleState",
    "setVisibleSelected",
    "setColorScheme",
    "setColorSchemeSelected",
    "setRepresentation",
    "setRepresentationSelected",
    "setColorLayout",
    "setGraphicsConfig",
):
    require(not hasattr(vtx, removed_name), f"Duplicate module binding should be removed: {removed_name}")

for enum_name in (
    "REPRESENTATION_VALUE",
    "SES_COMPUTE_MODE",
    "COLOR_LAYOUT_INDEX",
    "GRAPHICS_CONFIG_VALUE",
    "SHADING_MODE",
    "BACKGROUND_MODE",
    "SSAO_METHOD",
    "TONE_MAPPING",
):
    require(hasattr(vtx, enum_name), f"Missing preset enum: {enum_name}")

representation_name = "Python smoke representation"
vtx.createRepresentationPreset(representation_name)
representation = vtx.getRepresentationPreset(representation_name.lower())
require(
    representation.name == representation_name
    and any(
        preset == representation for preset in vtx.getRepresentationPresets()
    ),
    "Unexpected representation preset creation",
)
set_and_require_properties(
    representation,
    (
        ("hasSphere", False),
        ("radiusSphereFixed", 0.75),
        ("radiusSphereAdd", 0.25),
        ("isRadiusSphereFixed", True),
        ("hasCylinder", True),
        ("radiusCylinder", 0.2),
        ("cylinderColorBlending", True),
        ("hasRibbon", True),
        ("ribbonColorBlending", False),
        ("hasSes", True),
        ("sesComputeMode", vtx.SES_COMPUTE_MODE.POLYMER),
        ("sesProbeRadius", 1.5),
    ),
    "representation preset",
)
representation_renamed = representation_name + " renamed"
representation.rename(representation_renamed)
require(representation.name == representation_renamed, "Unexpected representation rename")
representation.duplicate()
representation_copy = vtx.getRepresentationPreset(representation_renamed + "_copy")
require(
    almost_equal(representation_copy.radiusCylinder, representation.radiusCylinder),
    "Unexpected representation duplicate",
)
representation_copy.delete()

color_layout_name = "Python smoke color layout"
vtx.createColorLayoutPreset(color_layout_name)
color_layout = vtx.getColorLayoutPreset(color_layout_name.lower())
require(
    color_layout.name == color_layout_name
    and color_layout.colorCount == len(color_layout)
    and any(preset == color_layout for preset in vtx.getColorLayoutPresets()),
    "Unexpected color layout preset creation",
)
layout_color = vtx.Rgba(0.15, 0.25, 0.35, 0.45)
color_layout[0] = layout_color
require(
    rgba_equal(color_layout.getColor(0), layout_color)
    and rgba_equal(color_layout[-color_layout.colorCount], layout_color),
    "Unexpected color layout item update",
)
layout_colors = color_layout.colors
layout_colors[-1] = vtx.Rgba(0.55, 0.65, 0.75, 0.85)
color_layout.colors = layout_colors
require(
    rgba_equal(color_layout[-1], layout_colors[-1]),
    "Unexpected complete color layout update",
)
color_layout.randomize()
require(len(color_layout.colors) == color_layout.colorCount, "Unexpected randomized color layout")
color_layout_renamed = color_layout_name + " renamed"
color_layout.rename(color_layout_renamed)
require(color_layout.name == color_layout_renamed, "Unexpected color layout rename")
color_layout_copy_name = color_layout_name + " duplicate"
color_layout.duplicate(color_layout_copy_name)
color_layout_copy = vtx.getColorLayoutPreset(color_layout_copy_name)
require(
    color_layout_copy.colorCount == color_layout.colorCount,
    "Unexpected color layout duplicate",
)
color_layout_copy.delete()

graphics_config_name = "Python smoke graphics config"
vtx.createGraphicsConfigPreset(graphics_config_name)
graphics_config = vtx.getGraphicsConfigPreset(graphics_config_name.lower())
require(
    graphics_config.name == graphics_config_name
    and any(
        preset == graphics_config for preset in vtx.getGraphicsConfigPresets()
    ),
    "Unexpected graphics config preset creation",
)
set_and_require_properties(
    graphics_config,
    (
        ("shadingMode", vtx.SHADING_MODE.FLAT),
        ("lightIntensity", 1.5),
        ("ambientIntensity", 0.2),
        ("backgroundMode", vtx.BACKGROUND_MODE.COLOR),
        ("specularFactor", 0.3),
        ("shininess", 24.0),
        ("toonSteps", 5),
        ("environmentPath", None),
        ("skyboxIntensity", 0.8),
        ("iblIntensity", 0.7),
        ("environmentRotation", 0.5),
        ("toneMappingMode", vtx.TONE_MAPPING.ACES),
        ("toneMappingExposure", 1.25),
        ("activeSsao", True),
        ("ssaoMethod", vtx.SSAO_METHOD.SAO),
        ("ssaoIntensity", 2.0),
        ("ssaoScale", 1.0),
        ("blurSize", 9.0),
        ("activeFog", True),
        ("fogNear", 10.0),
        ("fogFar", 100.0),
        ("fogDensity", 0.05),
        ("activeOutline", True),
        ("outlineSensitivity", 0.4),
        ("outlineThickness", 2),
        ("activeChromaticAberration", True),
        ("chromaticAberrationRed", 0.1),
        ("chromaticAberrationGreen", 0.2),
        ("chromaticAberrationBlue", 0.3),
        ("activePixelize", True),
        ("pixelizeSize", 4),
        ("pixelizeBackground", True),
        ("activeCrt", True),
        ("crtCurvatureX", 0.1),
        ("crtCurvatureY", 0.2),
        ("crtRatio", 1.1),
        ("crtGraninessX", 0.3),
        ("crtGraninessY", 0.4),
        ("crtVignetteRoundness", 0.5),
        ("crtVignetteIntensity", 0.6),
        ("crtBrightness", 0.9),
        ("activeSelection", True),
    ),
    "graphics config preset",
)
for property_name, expected in (
    ("colorLight", vtx.Rgba(0.1, 0.2, 0.3, 1.0)),
    ("colorBackground", vtx.Rgba(0.2, 0.3, 0.4, 1.0)),
    ("colorFog", vtx.Rgba(0.3, 0.4, 0.5, 1.0)),
    ("colorOutline", vtx.Rgba(0.4, 0.5, 0.6, 1.0)),
    ("colorSelection", vtx.Rgba(0.5, 0.6, 0.7, 1.0)),
):
    setattr(graphics_config, property_name, expected)
    require(
        rgba_equal(getattr(graphics_config, property_name), expected),
        f"Unexpected graphics config color: {property_name}",
    )
graphics_config_renamed = graphics_config_name + " renamed"
graphics_config.rename(graphics_config_renamed)
require(
    graphics_config.name == graphics_config_renamed,
    "Unexpected graphics config rename",
)
graphics_config_copy_name = graphics_config_name + " duplicate"
graphics_config.duplicate(graphics_config_copy_name)
graphics_config_copy = vtx.getGraphicsConfigPreset(graphics_config_copy_name)
require(
    graphics_config_copy.shadingMode == graphics_config.shadingMode,
    "Unexpected graphics config duplicate",
)
graphics_config_copy.delete()

representation.delete()
color_layout.delete()
graphics_config.delete()
require(
    all(preset != representation for preset in vtx.getRepresentationPresets())
    and all(preset != color_layout for preset in vtx.getColorLayoutPresets())
    and all(preset != graphics_config for preset in vtx.getGraphicsConfigPresets()),
    "Unexpected preset deletion",
)

position = vtx.Vec3f(1.0, 2.0, 3.0)
rotation = vtx.Quatf.fromEuler(0.0, 90.0, 0.0)

vtx.camera.setPosition(position)
vtx.camera.setRotation(rotation)
vtx.camera.setFov(45.0)
vtx.camera.setNearClip(0.1)
vtx.camera.setFarClip(1000.0)
vtx.camera.setProjectionOrthographic()
require(
    vtx.camera.projection == vtx.CAMERA_PROJECTION.ORTHOGRAPHIC
    and vtx.camera.isOrthographic(),
    "Camera should use orthographic projection",
)
vtx.camera.setProjectionPerspective()
require(
    vtx.camera.projection == vtx.CAMERA_PROJECTION.PERSPECTIVE
    and vtx.camera.isPerspective(),
    "Camera should use perspective projection",
)

camera_position = vtx.camera.position
camera_rotation = vtx.camera.rotation
require(almost_equal(camera_position.x, position.x), "Unexpected camera position x")
require(almost_equal(camera_position.y, position.y), "Unexpected camera position y")
require(almost_equal(camera_position.z, position.z), "Unexpected camera position z")
require(
    almost_equal(camera_rotation.w, rotation.w, 1e-4)
    and almost_equal(camera_rotation.y, rotation.y, 1e-4),
    "Unexpected camera rotation",
)
require(almost_equal(vtx.camera.fov, 45.0), "Unexpected camera field of view")
require(almost_equal(vtx.camera.nearClip, 0.1), "Unexpected camera near clipping plane")
require(almost_equal(vtx.camera.farClip, 1000.0), "Unexpected camera far clipping plane")

try:
    from pathlib import Path

    system_path = str(Path(__file__).with_name("1AGA.mmtf"))
except NameError:
    system_path = "data/1AGA.mmtf"

vtx.openFile(system_path)
topology_system = vtx.getSystem("1aga")
system = topology_system
require(vtx.getSystem("1aga.MMTF") == system, "File name lookup should ignore case")
require(vtx.getSystem("1AGA") == system, "File stem lookup should ignore case")
require(
    vtx.getSystem("the agarose double helix and its function in agarose gel structure")
    == system,
    "System name lookup should ignore case",
)
require(
    len({topology_system, vtx.getSystem("1AGA")}) == 1,
    "Systems referencing the same entity should hash and compare equal",
)
require(not hasattr(topology_system, "id"), "System must not expose its entity id")
loaded_systems = vtx.getSystems()
require(
    len(loaded_systems) == 1 and loaded_systems[0] == system,
    "Unexpected loaded systems",
)
require(
    topology_system.name.lower()
    == "the agarose double helix and its function in agarose gel structure",
    "Unexpected topology system name",
)
system_name = topology_system.name
topology_system.setName("Python smoke system")
require(topology_system.name == "Python smoke system", "Unexpected renamed system name")
require(vtx.getSystem("python smoke system") == topology_system, "Renamed system lookup should ignore case")
topology_system.setName(system_name)
require(topology_system.name == system_name, "System name should be restored")

system_position = topology_system.position
system_rotation = topology_system.rotation
system_scale = topology_system.scale
topology_system.setPosition(vtx.Vec3f(1.0, 2.0, 3.0))
topology_system.setRotation(vtx.Quatf.fromEuler(0.0, 90.0, 0.0))
topology_system.setScale(vtx.Vec3f(2.0, 2.0, 2.0))
moved_position = topology_system.position
moved_rotation = topology_system.rotation
moved_scale = topology_system.scale
require(
    almost_equal(moved_position.x, 1.0)
    and almost_equal(moved_position.y, 2.0)
    and almost_equal(moved_position.z, 3.0),
    "Unexpected system position",
)
require(
    almost_equal(abs(moved_rotation.w), 0.70710678, 1e-4)
    and almost_equal(abs(moved_rotation.y), 0.70710678, 1e-4),
    "Unexpected system rotation",
)
require(
    almost_equal(moved_scale.x, 2.0)
    and almost_equal(moved_scale.y, 2.0)
    and almost_equal(moved_scale.z, 2.0),
    "Unexpected system scale",
)
topology_system.setPosition(system_position)
topology_system.setRotation(system_rotation)
topology_system.setScale(system_scale)

require("1aga" in topology_system.path.lower(), "Unexpected topology system path")
require(topology_system.pdbIdCode.lower() == "1aga", "Unexpected topology system PDB id")
require(
    topology_system.chainCount == len(topology_system.getChains())
    and topology_system.residueCount == len(topology_system.getResidues())
    and topology_system.atomCount == len(topology_system.getAtoms())
    and topology_system.bondCount == len(topology_system.getBonds())
    and topology_system.categoryCount == len(topology_system.getCategories()),
    "Unexpected topology system counts",
)
require(
    topology_system.indexFirstChain == 0
    and topology_system.indexLastChain == topology_system.chainCount
    and topology_system.indexFirstResidue == 0
    and topology_system.indexLastResidue == topology_system.residueCount
    and topology_system.indexFirstAtom == 0
    and topology_system.indexLastAtom == topology_system.atomCount
    and topology_system.indexFirstBond == 0
    and topology_system.indexLastBond == topology_system.bondCount,
    "Unexpected topology system ranges",
)

topology_chains = topology_system.getChains()
topology_residues = topology_system.getResidues()
topology_atoms = topology_system.getAtoms()
topology_bonds = topology_system.getBonds()
topology_categories = topology_system.getCategories()
require(topology_chains[0].name != "", "Unexpected chain collection item")
require(topology_residues[0].shortName != "", "Unexpected residue collection item")
require(topology_atoms[0].vdwRadius >= 0.0, "Unexpected atom collection item")
require(topology_bonds[0].order != "", "Unexpected bond collection item")
require(topology_categories[0].name != "", "Unexpected category collection item")
require(len(topology_chains[0:1]) == 1, "Unexpected chain collection slice")
require(len(topology_residues[0:1]) == 1, "Unexpected residue collection slice")
require(len(topology_atoms[0:1]) == 1, "Unexpected atom collection slice")
require(len(topology_bonds[0:1]) == 1, "Unexpected bond collection slice")
require(len(topology_categories[0:1]) == 1, "Unexpected category collection slice")

topology_chain = topology_system.getChain(0)
require(topology_chain.name != "", "Unexpected chain name")
require(topology_chain.getSystem() == system, "Unexpected chain system")
require(
    topology_chain.residueCount == len(topology_chain.getResidues())
    and topology_chain.atomCount == len(topology_chain.getAtoms()),
    "Unexpected chain counts",
)
require(
    topology_chain.indexLastResidue
    == topology_chain.indexFirstResidue + topology_chain.residueCount
    and topology_chain.indexLastAtom
    == topology_chain.indexFirstAtom + topology_chain.atomCount,
    "Unexpected chain ranges",
)

topology_residue = topology_chain.getResidue(0)
require(
    topology_residue.name != ""
    and topology_residue.symbol is not None
    and topology_residue.shortName != ""
    and topology_residue.longName != ""
    and topology_residue.atomCount == len(topology_residue.getAtoms()),
    "Unexpected residue metadata",
)
require(
    topology_residue.indexLastAtom
    == topology_residue.indexFirstAtom + topology_residue.atomCount,
    "Unexpected residue ranges",
)
topology_residue_category = topology_residue.getCategory()
require(topology_residue_category.index >= 0, "Unexpected residue category")
require(topology_residue.getSystem() == system, "Unexpected residue system")

topology_atom = topology_residue.getAtom(0)
require(
    topology_atom.name != ""
    and topology_atom.symbol is not None
    and topology_atom.symbolName != "",
    "Unexpected atom metadata",
)
require(
    topology_atom.vdwRadius >= 0.0,
    "Unexpected atom type metadata",
)
topology_atom_category = topology_atom.getCategory()
require(topology_atom_category.index >= 0, "Unexpected atom category")
require(topology_atom.getSystem() == system, "Unexpected atom system")
topology_atom_position = topology_atom.position
topology_atom_aabb = topology_residue.getAtoms()[0:1].aabb
require(
    topology_atom_aabb.valid
    and topology_atom_aabb.min.x <= topology_atom_position.x <= topology_atom_aabb.max.x
    and topology_atom_aabb.min.y <= topology_atom_position.y <= topology_atom_aabb.max.y
    and topology_atom_aabb.min.z <= topology_atom_position.z <= topology_atom_aabb.max.z,
    "Unexpected atom position or AABB",
)

for play_mode in (
    vtx.TRAJECTORY_PLAY_MODE.NONE,
    vtx.TRAJECTORY_PLAY_MODE.FORWARD,
    vtx.TRAJECTORY_PLAY_MODE.FORWARD_LOOP,
    vtx.TRAJECTORY_PLAY_MODE.BACKWARD_LOOP,
    vtx.TRAJECTORY_PLAY_MODE.BACKWARD,
    vtx.TRAJECTORY_PLAY_MODE.PING_PONG,
):
    require(isinstance(play_mode, vtx.TRAJECTORY_PLAY_MODE), "Unexpected trajectory play mode")
trajectory = topology_system.trajectory
require(
    trajectory.frameCount == 1
    and trajectory.loadedFrameCount == 1
    and len(trajectory) == 1
    and trajectory.currentFrameIndex == 0,
    "Unexpected single-frame trajectory",
)
require(
    trajectory.availableFrames.first == 0
    and trajectory.availableFrames.last == 1
    and trajectory.availableFrames.count == 1,
    "Unexpected available frames",
)
require(
    trajectory.requestedFrameIndex == 0
    and trajectory.playMode == vtx.TRAJECTORY_PLAY_MODE.NONE
    and trajectory.paused
    and almost_equal(trajectory.speed, 0.0),
    "Unexpected single-frame player state",
)
frame = topology_system.currentFrame
require(frame.index == 0 and len(frame) == topology_system.atomCount, "Unexpected frame")
require(trajectory.currentFrame.index == 0, "Unexpected trajectory current frame")
require(trajectory.getFrame(0).index == 0 and trajectory[0].index == 0, "Unexpected trajectory indexing")
for trajectory_command in (
    lambda: trajectory.toggleStartPause(),
    lambda: trajectory.play(),
    lambda: trajectory.pause(),
    lambda: trajectory.stop(),
    lambda: trajectory.jumpTo(0),
    lambda: trajectory.setPlayMode(vtx.TRAJECTORY_PLAY_MODE.NONE),
    lambda: trajectory.setSpeed(35.0),
):
    try:
        trajectory_command()
    except ValueError:
        pass
    else:
        raise AssertionError("A single-frame trajectory command should raise ValueError")
frame_atom_position = frame[topology_atom.index]
direct_frame_atom_position = frame.getAtomPosition(topology_atom.index)
require(
    almost_equal(frame_atom_position.x, topology_atom_position.x)
    and almost_equal(frame_atom_position.y, topology_atom_position.y)
    and almost_equal(frame_atom_position.z, topology_atom_position.z),
    "Unexpected frame atom position",
)
require(
    almost_equal(direct_frame_atom_position.x, topology_atom_position.x),
    "Unexpected direct frame atom position",
)
topology_category = topology_system.getCategory(0)
require(
    topology_category.name != ""
    and topology_category.residueCount == len(topology_category.getResidues())
    and topology_category.atomCount == len(topology_category.getAtoms()),
    "Unexpected category metadata",
)
require(topology_category.getSystem() == system, "Unexpected category system")

topology_bond = topology_system.getBond(0)
require(topology_bond.getSystem() == system, "Unexpected bond system")
require(
    topology_system.getChain(0) == topology_chain
    and topology_chain.getResidue(0) == topology_residue
    and topology_residue.getAtom(0) == topology_atom
    and topology_system.getCategory(0) == topology_category
    and topology_system.getBond(topology_bond.index) == topology_bond
    and topology_system.getResidue(1) != topology_residue
    and topology_system.getChain(0) != topology_system.getResidue(0)
    and len({topology_atom, topology_residue.getAtom(0)}) == 1,
    "Unexpected topology handle equality",
)

for topology_item in (
    topology_system,
    topology_chains,
    topology_residues,
    topology_atoms,
    topology_bonds,
    topology_categories,
):
    require(topology_item.aabb.valid, "Unexpected topology AABB")

vtx.selectAll()
require(topology_system.selectionState == vtx.SELECTION_STATE.FULL, "System should be selected")
require(
    topology_system.isSelected() and topology_system.isFullySelected(),
    "Topology system should be selected",
)
require(
    len(vtx.selection.getSystems()) == 1
    and len(vtx.selection.getChains(topology_system)) == topology_system.chainCount
    and len(vtx.selection.getResidues(topology_system)) == topology_system.residueCount
    and len(vtx.selection.getAtoms(topology_system)) == topology_system.atomCount,
    "Unexpected complete selection",
)
topology_system.clearSelection()
require(topology_system.selectionState == vtx.SELECTION_STATE.NONE, "System selection should be cleared")
require(
    not topology_system.isSelected() and not topology_system.isFullySelected(),
    "Topology system selection should be cleared",
)
require(vtx.selection.isEmpty(topology_system), "System selection should be empty")
require(len(vtx.selection.getSystems()) == 0, "Selected system collection should be empty")

topology_system.getResidues([0, 1, 2]).setSelected()
require(
    topology_system.getResidue(0).selectionState == vtx.SELECTION_STATE.FULL,
    "Residue 0 should be selected",
)
require(
    topology_system.getResidue(3).selectionState == vtx.SELECTION_STATE.NONE,
    "Residue 3 should not be selected",
)
require(
    [residue.index for residue in topology_system.getResidues((0, 2))] == [0, 2]
    and [atom.index for atom in topology_system.getAtoms(vtx.Range(1, 3))] == [1, 2],
    "Unexpected collection built from indices",
)
try:
    topology_system.getResidues([topology_system.residueCount])
except IndexError:
    pass
else:
    raise AssertionError("Out of range residue indices should raise IndexError")
require(
    topology_residue.isSelected() and topology_residue.isFullySelected(),
    "Topology residue should be selected",
)

selected_systems = vtx.selection.getSystems()
selected_residues = vtx.selection.getResidues(topology_system)
selected_atoms = vtx.selection.getAtoms(topology_system)
selected_chains = vtx.selection.getChains(topology_system)
selected_categories = vtx.selection.getCategories(topology_system)

expected_atom_indices = [
    atom.index
    for residue_index in range(3)
    for atom in topology_system.getResidue(residue_index).getAtoms()
]
expected_chain_indices = sorted(
    {topology_system.getAtom(index).getChain().index for index in expected_atom_indices}
)
expected_category_indices = sorted(
    {topology_system.getAtom(index).getCategory().index for index in expected_atom_indices}
)

require(
    len(selected_systems) == 1 and selected_systems[0] == system,
    "Unexpected selected system collection",
)
require(not vtx.selection.isEmpty(topology_system), "System selection should not be empty")
require(
    [residue.index for residue in selected_residues] == [0, 1, 2],
    "Unexpected selected residue collection",
)
require(
    [atom.index for atom in selected_atoms] == expected_atom_indices,
    "Unexpected selected atom collection",
)
require(
    [chain.index for chain in selected_chains] == expected_chain_indices,
    "Unexpected selected chain collection",
)
require(
    [category.index for category in selected_categories] == expected_category_indices,
    "Unexpected selected category collection",
)
require(
    selected_residues[0].index == 0 and len(selected_residues[0:2]) == 2,
    "Unexpected selected residue collection access",
)

topology_system.setColorScheme(vtx.COLOR_SCHEME.CHAIN)
topology_system.getResidues(vtx.RangeList(vtx.Range(0, 2))).setColorScheme(
    vtx.COLOR_SCHEME.RESIDUE
)
topology_system.getResidues([0, 1]).setColorScheme(vtx.COLOR_SCHEME.CUSTOM, 228)
topology_chain.setColorScheme(vtx.COLOR_SCHEME.ATOM)
topology_residue.setColorScheme(vtx.COLOR_SCHEME.RESIDUE)
topology_atom.setColorScheme(vtx.COLOR_SCHEME.CUSTOM, 228)
topology_category.setColorScheme(vtx.COLOR_SCHEME.CHAIN)
try:
    topology_system.setColorScheme(vtx.COLOR_SCHEME.CUSTOM)
except ValueError:
    pass
else:
    raise AssertionError("A custom color scheme without color index should raise ValueError")
vtx.selection.setColorScheme(vtx.COLOR_SCHEME.CUSTOM, 228)
vtx.selection.setColorScheme(vtx.COLOR_SCHEME.RESIDUE)
topology_system.setSecondaryStructureColorScheme(
    vtx.SECONDARY_STRUCTURE_COLOR_SCHEME.STRUCTURE
)
topology_residues.setSecondaryStructureColorScheme(
    vtx.SECONDARY_STRUCTURE_COLOR_SCHEME.STRUCTURE
)
vtx.getRepresentationPreset("sticks").apply()
topology_system.setRepresentation("sticks and ribbons")
topology_system.getResidues(vtx.Range(0, 2)).setRepresentation("sticks")
topology_chain.setRepresentation("sticks")
representation_preset = vtx.getRepresentationPreset("sticks")
topology_system.setRepresentation(representation_preset)
topology_system.getResidues(vtx.Range(0, 2)).setRepresentation(representation_preset)
vtx.selection.setRepresentation("sticks")
vtx.selection.setRepresentation(representation_preset)
vtx.selection.show()
vtx.selection.hide()

topology_residue.setRepresentation(representation_preset)
vtx.selection.hide()
require(
    topology_system.getResidue(0).visibleState == vtx.VISIBLE_STATE.HIDDEN,
    "Selected residue should be hidden",
)
require(
    not topology_residue.isVisible() and not topology_residue.isFullyVisible(),
    "Topology residue should be hidden",
)
require(
    topology_system.getResidue(3).visibleState == vtx.VISIBLE_STATE.VISIBLE,
    "Unselected residue should remain visible",
)

topology_system.getResidues([0, 1, 2]).setVisible(True)
require(
    topology_system.getResidue(0).visibleState == vtx.VISIBLE_STATE.VISIBLE,
    "Residue should be visible",
)

vtx.hideAll()
require(topology_system.visibleState == vtx.VISIBLE_STATE.HIDDEN, "System should be hidden")
require(
    not topology_system.isVisible() and not topology_system.isFullyVisible(),
    "Topology system should be hidden",
)
vtx.showAll()
require(topology_system.visibleState == vtx.VISIBLE_STATE.VISIBLE, "System should be visible")
require(
    topology_system.isVisible() and topology_system.isFullyVisible(),
    "Topology system should be visible",
)

vtx.camera.orient()
vtx.camera.orient(topology_system.aabb)
vtx.camera.orient(topology_system)
vtx.camera.orient(topology_chain)
vtx.camera.orient(topology_residues)
vtx.camera.straightTravel(position, rotation, 200.0)
travel_position = vtx.camera.position
require(
    almost_equal(travel_position.x, position.x, 1e-3)
    and almost_equal(travel_position.y, position.y, 1e-3)
    and almost_equal(travel_position.z, position.z, 1e-3),
    "Animated straight travel should wait for the end of the animation",
)
vtx.camera.orient(topology_system, duration=200.0)
vtx.camera.reset()

viewpoint_count = len(vtx.camera.getViewpoints())
viewpoint = vtx.camera.saveViewpoint("Python smoke viewpoint")
require(viewpoint.name == "Python smoke viewpoint", "Unexpected viewpoint name")
require(vtx.camera.getViewpoint("python smoke viewpoint") == viewpoint, "Viewpoint lookup should ignore case")
require(len(vtx.camera.getViewpoints()) == viewpoint_count + 1, "Viewpoint should be listed")
require(
    len({viewpoint, vtx.camera.getViewpoint("Python smoke viewpoint")}) == 1,
    "Viewpoints referencing the same entity should hash and compare equal",
)
default_viewpoint = vtx.camera.saveViewpoint()
require(default_viewpoint != viewpoint and default_viewpoint.name == "Viewpoint", "Unexpected default viewpoint")
default_viewpoint.delete()
require(len(vtx.camera.getViewpoints()) == viewpoint_count + 1, "Deleted viewpoint should not be listed")
try:
    default_viewpoint.goTo()
except ValueError:
    pass
else:
    raise AssertionError("Deleted viewpoint should raise ValueError")

viewpoint.setName("Python smoke viewpoint renamed")
viewpoint.setPosition(vtx.Vec3f(4.0, 5.0, 6.0))
viewpoint.setRotation(vtx.Quatf.fromEuler(0.0, 90.0, 0.0))
viewpoint_position = viewpoint.position
viewpoint_rotation = viewpoint.rotation
require(viewpoint.name == "Python smoke viewpoint renamed", "Unexpected renamed viewpoint")
require(
    almost_equal(viewpoint_position.x, 4.0)
    and almost_equal(viewpoint_position.y, 5.0)
    and almost_equal(viewpoint_position.z, 6.0),
    "Unexpected viewpoint position",
)
require(
    almost_equal(abs(viewpoint_rotation.w), 0.70710678, 1e-4)
    and almost_equal(abs(viewpoint_rotation.y), 0.70710678, 1e-4),
    "Unexpected viewpoint rotation",
)
for duration in (0.0, 200.0):
    vtx.camera.reset()
    viewpoint.goTo(duration=duration)
    goto_position = vtx.camera.position
    require(
        almost_equal(goto_position.x, 4.0, 1e-3)
        and almost_equal(goto_position.y, 5.0, 1e-3)
        and almost_equal(goto_position.z, 6.0, 1e-3),
        f"Camera should be at the viewpoint after goTo(duration={duration})",
    )
viewpoint.updateFromCamera()
viewpoint.target
viewpoint.goTo()
viewpoint.delete()
require(len(vtx.camera.getViewpoints()) == viewpoint_count, "Viewpoint should be deleted")

original_resolution = vtx.getResolution()
vtx.setResolution(640, 480)
require(vtx.getResolution() == (640, 480), "Unexpected resolution")
try:
    vtx.setResolution(0, 480)
except ValueError:
    pass
else:
    raise AssertionError("A zero resolution should raise ValueError")
vtx.setResolution(*original_resolution)
require(vtx.getResolution() == original_resolution, "Resolution should be restored")

vtx.clearSelection()
require(topology_system.selectionState == vtx.SELECTION_STATE.NONE, "Selection should be cleared")
require(vtx.selection.isEmpty(topology_system), "System selection should be empty")
require(len(vtx.selection.getSystems()) == 0, "Selected system collection should be empty")
require(len(vtx.selection.getAtoms(topology_system)) == 0, "Selected atom collection should be empty")
require(len(vtx.selection.getResidues(topology_system)) == 0, "Selected residue collection should be empty")
require(len(vtx.selection.getChains(topology_system)) == 0, "Selected chain collection should be empty")
require(len(vtx.selection.getCategories(topology_system)) == 0, "Selected category collection should be empty")

vtx.quit()

print("Python binding smoke test completed")
