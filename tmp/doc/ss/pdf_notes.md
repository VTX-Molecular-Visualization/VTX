# Secondary structure papers

## Adaptive on-the-fly molecular ribbons generation

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

## Knowledge-Based Protein Secondary Structure Assignment

## Visualization of complex molecular ribbon structures at interactive rates (2004)

Based on combined BReps, a mesh structure defining a combination of polygonal faces and Catmull/Clark surfaces.

## Instant visualization of secondary structures of molecular models

## Dictionary of Protein Secondary Structure: Pattern Recognition of Hydrogen-Bonded and Geometrical Features

## Interactive Visualization of RNA and DNA Structures

## Schematic Drawings of Protein Structures

## GPU-based Visualisation of Protein Secondary Structure

## GPU-Accelerated Visualization of Protein Dynamics in Ribbon Mode