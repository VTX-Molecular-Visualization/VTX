# Secondary structure notes

## PDB format

### Data types

3 data types possible, here is the mapping from PDB type to SS type:

- HELIX -> HELIX

- SHEET -> STRAND

- TURN -> COIL

### HELIX idendification code

- 1 = left alpha

- 2 = left omega

- 3 = left pi

- 4 = left gamma

- 5 = 3-10

- 6 = right alpha

- 7 = right omega

- 8 = right gamma 

## Stride algorithm

### From UnityMol

On récupère les atomes :

- C du résidu précédent (C0)
- N, CA, C du résident courant (N1, CA1, C1)
- N du résidu suivant (N2)

On calcule l'angle dièdre entre C0 N1 CA1 C1.

On calcule l'angle dièdre entre N1 CA1 C1 N2.

Pour le premier résidu, l'angle est défini à 90°.