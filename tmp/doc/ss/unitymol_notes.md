# UnityMol notes

## Construction

Construction des peptide planes avec les atomes CA et O du résidu courant et CA du résidu suivant.

## Stride algorithm

### From UnityMol

On récupère les atomes :

- C du résidu précédent (C0)
- N, CA, C du résident courant (N1, CA1, C1)
- N du résidu suivant (N2)

On calcule l'angle dièdre entre C0 N1 CA1 C1.

On calcule l'angle dièdre entre N1 CA1 C1 N2.

Pour le premier résidu, l'angle est défini à 90°.

// TOFINISH

