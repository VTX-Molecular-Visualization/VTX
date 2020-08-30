# Secondary structure papers

## Dictionary of Protein Secondary Structure: Pattern Recognition of Hydrogen-Bonded and Geometrical Features (1983)

DSSP

## STRIDE: a web server for secondary structure assignment from known atomic coordinates of proteins (2004)

## Schematic Drawings of Protein Structures (1985)

## Algorithm for ribbon models of proteins (1985)

Only construction, not computation, base on B-spline.

### Peptide plane

- Vector A = CA(i+1) - CA(i)
- Vector B = O(i) - CA(i)
- Vector C = A x B
- Vector D = C x A
- Normalize C and D

Vector C is normal to the peptide plane approximated by atoms CA(i), O(i) and CA(i+1).
Handedness has vector C pointing away from the helix axis for right-handed alpha helices.
Vector D lies parallel to the peptide plane and is perpendicular to A.

### B-spline control points

- Point P is the middle point of vector CA(i) -> CA(i+1)
- If the peptide plane is part of a helix, translate in the direction of vector C, away from the helix axis
- Scale vector D by one half the desired ribbon width
- Point P- = P - D
- Point P+ = P + D

The segment P- -> P+ is of the desired ribbon width and parallel to the peptide plane.
The guide coordinates are evenly spaced along this line segment.
A set of guide points is then generated from the next peptide plane in the sequence.

If the angle between D(i-1) and D(i) is greater than 90°, a flip is noted (always the case between adjacent residues of B sheets).
This information is used to determine if the line segment defined in the previous step will have its first point, P-, or its last point, P+, assigned to the first thread of the ribbon.

It must be noted that the curves are very smooth (10 points between control points), and do not necessarily pass through their guide coordinates (except for the first and last points).

## Knowledge-Based Protein Secondary Structure Assignment (1995)

## Visualization of complex molecular ribbon structures at interactive rates (2004)

Based on combined BReps, a mesh structure defining a combination of polygonal faces and Catmull/Clark surfaces.
A protein is composed of possibly more than one chain of amino acids linked together by peptide bonds.
The backbone of a chain is formed by three groups of atoms in each amino acid, namely the central atom Cα, the amino groupN−Hand the carbonyl group N=O.
There are 20 amino acids with associated characters.
A protein can be described by the sequence of characterd = primary structure.
Since the peptide bond is not rigid, the protein can rotate around this bond to fold itself to an energetically convenient state = teritiary structure.
Secondary structure = alpha helix, beta sheet and some random turns.
Secondary and tertiary are visualized with a ribbon (Richardson 1981).
Ribbons of 2 chains are not connected.

These are smooth, dart, crease, and corner vertices, whereby a smooth vertex has no sharp edge and a corner vertex has three or more sharp edges, resulting to this face classification:
- Smooth face: A face with at least one smooth edge
- Sharp face: A planar face with only sharp edges, but the vertices may be crease vertices
- Polygonal face: Like sharp face but all vertices are corner vertices

// End of the paper talk about mathematical construction.

## GPU-based Visualisation of Protein Secondary Structure (2008)

All amino acids have an identical part, called backbone, which consists of the amino and the carboxyl functional group, which are attached to another carbon, the so-called a-carbon.
The amino acids can form bonds between each other like hydrogen bonds, salt bridges and disulphide bonds.
That way the strand folds in a specific manner according to the energetic minimum. The folding is highly stabilised by these linkages.
This conformation is called the protein’s secondary structure.
Folding is essential for the protein’s functionality: many diseases like Alzheimer’s or BSE are caused by misfolded proteins.

Stride = DSSP (atomic coordinates and hydrogen bond energy) + dihedral angles.

The cartoon representation visualises the secondary structure of a protein. There are special graphical representations for the different kinds of secondary structure elements.
As can be seen in Figure 4 the random coil and turns are visualised as tubes, the a-helices are depicted using broad ribbons that coil following the turns of the helices and the b-sheets are drawn as flat, band-shaped arrows that point in the direction of the carboxyl group.

## GPU-Accelerated Visualization of Protein Dynamics in Ribbon Mode (2011)

## Instant visualization of secondary structures of molecular models (2015)

In this paper we present an adaptive system that generates high quality shapes using the tessellation shader.
Amino acids are connected between them by peptide bonds.
There are 23 types of amino acids with a common structure (backbone): H, N, C, C and O + a different residue.
The peptide bond can fold.
This tridimensional structure may cause two non-consecutive amino acids to create a hydrogen bond between them leading to characteristic structures. These structures are known as secondary structures and the most common are:
- a-helix: A part of the backbone acquires a form of an helix.
- b-sheet: Two distant parts of the backbone are interconnected in parallel.

Arrows are on last b-sheets.

Compute only BSpline and some vectors on CPU to render ribbon on GPU.
BSpline control points are simply consecutive Cα.
On CPU, compute a vertex buffer (size = residue count) with Cα positions, another with the vector direction, and a last with the secondary structure type.
In the index buffer, store four indexes for each segment of the B-Spline.
To draw the other side, add all the index in the reversed order to the index buffer.
Only vertex buffer change during dynamic.
// TODO: determine arrows in buffer (g pa kompri).

// TODO: shaders.

## Adaptive on-the-fly molecular ribbons generation (2015)

## Interactive Visualization of RNA and DNA Structures (2018)