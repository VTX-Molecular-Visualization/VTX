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

vtx.setCameraPosition(position)
vtx.setCameraRotation(rotation)
vtx.setCameraFov(45.0)
vtx.setCameraNearClip(0.1)
vtx.setCameraFarClip(1000.0)
vtx.setCameraProjectionOrthographic()
vtx.setCameraProjectionPerspective()

camera_position = vtx.getCameraPosition()
camera_rotation = vtx.getCameraRotation()
require(almost_equal(camera_position.x, position.x), "Unexpected camera position x")
require(almost_equal(camera_position.y, position.y), "Unexpected camera position y")
require(almost_equal(camera_position.z, position.z), "Unexpected camera position z")
require(
    almost_equal(camera_rotation.w, rotation.w), "Unexpected camera rotation w"
)

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
    topology_atom.type is not None and topology_atom.vdwRadius >= 0.0,
    "Unexpected atom type metadata",
)
topology_atom_category = topology_atom.getCategory()
require(topology_atom_category.index >= 0, "Unexpected atom category")
require(topology_atom.getSystem().id == system, "Unexpected atom system")

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

vtx.selectAll()
require(vtx.getSelectionState(system) == vtx.SELECTION_STATE.FULL, "System should be selected")
require(
    topology_system.isSelected() and topology_system.isFullySelected(),
    "Topology system should be selected",
)
vtx.clearSelection(system)
require(vtx.getSelectionState(system) == vtx.SELECTION_STATE.NONE, "System selection should be cleared")
require(
    not topology_system.isSelected() and not topology_system.isFullySelected(),
    "Topology system selection should be cleared",
)

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

vtx.orientCamera()
vtx.straightTravelCamera(position, rotation, 500.0)
vtx.resetCamera()

vtx.clearSelection()
require(vtx.getSelectionState(system) == vtx.SELECTION_STATE.NONE, "Selection should be cleared")

vtx.quit()

print("Python binding smoke test completed")
