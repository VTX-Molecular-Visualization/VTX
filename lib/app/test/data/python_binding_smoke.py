def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def almost_equal(left, right, tolerance=1e-6):
    return abs(left - right) <= tolerance


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

transform = vtx.Transform()
transform.position = vec3
transform.rotation = vtx.Quatf(0.0, 0.0, 0.0)
transform.scale = vtx.Vec3f(2.0, 2.0, 2.0)
require(
    almost_equal(transform.position.z, 3.0)
    and almost_equal(transform.rotation.w, 1.0)
    and almost_equal(transform.scale.x, 2.0),
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
    "deleteSystem",
    "deleteSystemSelected",
    "makeSnapshot",
    "associateTrajectory",
    "download",
):
    require(callable(getattr(vtx, function_name)), f"Missing binding: {function_name}")

position = vtx.Vec3f(1.0, 2.0, 3.0)
rotation = vtx.Quatf(0.0, 0.0, 0.0)

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
    almost_equal(camera_rotation.w, rotation.w), "Unexpected camera rotation w"
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
system = vtx.getSystemIdByPdb("1aga")
require(system == vtx.getSystemIdByFileName("1aga.MMTF"), "File name lookup should ignore case")
require(
    system
    == vtx.getSystemIdByName(
        "the agarose double helix and its function in agarose gel structure"
    ),
    "System name lookup should ignore case",
)

topology_system = vtx.getSystem("1aga")
require(topology_system.id == system, "Unexpected topology system id")
require(
    topology_system.name.lower()
    == "the agarose double helix and its function in agarose gel structure",
    "Unexpected topology system name",
)
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
require(topology_chain.getSystem().id == system, "Unexpected chain system")
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
require(topology_residue.getSystem().id == system, "Unexpected residue system")

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
require(topology_atom.getSystem().id == system, "Unexpected atom system")
topology_atom_position = topology_atom.position
topology_atom_aabb = topology_residue.getAtoms()[0:1].aabb
require(
    topology_atom_aabb.valid
    and topology_atom_aabb.min.x <= topology_atom_position.x <= topology_atom_aabb.max.x
    and topology_atom_aabb.min.y <= topology_atom_position.y <= topology_atom_aabb.max.y
    and topology_atom_aabb.min.z <= topology_atom_position.z <= topology_atom_aabb.max.z,
    "Unexpected atom position or AABB",
)

trajectory = topology_system.getTrajectory()
for play_mode in (
    vtx.TRAJECTORY_PLAY_MODE.NONE,
    vtx.TRAJECTORY_PLAY_MODE.FORWARD,
    vtx.TRAJECTORY_PLAY_MODE.FORWARD_LOOP,
    vtx.TRAJECTORY_PLAY_MODE.BACKWARD_LOOP,
    vtx.TRAJECTORY_PLAY_MODE.BACKWARD,
    vtx.TRAJECTORY_PLAY_MODE.PING_PONG,
):
    require(isinstance(play_mode, vtx.TRAJECTORY_PLAY_MODE), "Unexpected trajectory play mode")
require(not trajectory.isMultiFrame(), "Unexpected multi-frame trajectory")
require(
    trajectory.frameCount == 1
    and len(trajectory) == 1
    and trajectory.currentFrameIndex == 0,
    "Unexpected single-frame trajectory",
)
frame = trajectory.getCurrentFrame()
require(frame.index == 0 and len(frame) == topology_system.atomCount, "Unexpected frame")
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
require(
    trajectory.getFrame(0).index == 0 and trajectory[0].index == 0,
    "Unexpected trajectory indexing",
)

topology_category = topology_system.getCategory(0)
require(
    topology_category.name != ""
    and topology_category.residueCount == len(topology_category.getResidues())
    and topology_category.atomCount == len(topology_category.getAtoms()),
    "Unexpected category metadata",
)
require(topology_category.getSystem().id == system, "Unexpected category system")

topology_bond = topology_system.getBond(0)
require(topology_bond.getSystem().id == system, "Unexpected bond system")

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
require(vtx.getSelectionState(system) == vtx.SELECTION_STATE.FULL, "System should be selected")
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
vtx.clearSelection(system)
require(vtx.getSelectionState(system) == vtx.SELECTION_STATE.NONE, "System selection should be cleared")
require(
    not topology_system.isSelected() and not topology_system.isFullySelected(),
    "Topology system selection should be cleared",
)
require(vtx.selection.isEmpty(topology_system), "System selection should be empty")
require(len(vtx.selection.getSystems()) == 0, "Selected system collection should be empty")

vtx.select(system, vtx.SYSTEM_ITEM.RESIDUE, [0, 1, 2], True, False)
require(
    vtx.getSelectionState(system, vtx.SYSTEM_ITEM.RESIDUE, 0) == vtx.SELECTION_STATE.FULL,
    "Residue 0 should be selected",
)
require(
    vtx.getSelectionState(system, vtx.SYSTEM_ITEM.RESIDUE, 3) == vtx.SELECTION_STATE.NONE,
    "Residue 3 should not be selected",
)
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
    len(selected_systems) == 1 and selected_systems[0].id == system,
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
    not topology_residue.isVisible() and not topology_residue.isFullyVisible(),
    "Topology residue should be hidden",
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
require(
    not topology_system.isVisible() and not topology_system.isFullyVisible(),
    "Topology system should be hidden",
)
vtx.showAll()
require(vtx.getVisibleState(system) == vtx.VISIBLE_STATE.VISIBLE, "System should be visible")
require(
    topology_system.isVisible() and topology_system.isFullyVisible(),
    "Topology system should be visible",
)

vtx.camera.orient()
vtx.camera.orient(topology_system.aabb)
vtx.camera.orient(topology_system)
vtx.camera.orient(topology_chain)
vtx.camera.orient(topology_residues)
vtx.orientCamera(topology_system.aabb)
vtx.camera.straightTravel(position, rotation, 500.0)
vtx.camera.reset()

vtx.clearSelection()
require(vtx.getSelectionState(system) == vtx.SELECTION_STATE.NONE, "Selection should be cleared")
require(vtx.selection.isEmpty(topology_system), "System selection should be empty")
require(len(vtx.selection.getSystems()) == 0, "Selected system collection should be empty")
require(len(vtx.selection.getAtoms(topology_system)) == 0, "Selected atom collection should be empty")
require(len(vtx.selection.getResidues(topology_system)) == 0, "Selected residue collection should be empty")
require(len(vtx.selection.getChains(topology_system)) == 0, "Selected chain collection should be empty")
require(len(vtx.selection.getCategories(topology_system)) == 0, "Selected category collection should be empty")

vtx.quit()

print("Python binding smoke test completed")
