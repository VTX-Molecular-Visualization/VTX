#include "concept.hpp"

namespace VTX::Core::NewStruct
{
	template<ConceptMolecule M, ConceptResidue R>
	class Atom;

	template<ConceptMolecule M>
	class Bond;

	template<ConceptMolecule M, ConceptChain C>
	class Residue;

	template<ConceptMolecule M>
	class Chain;

	template<ConceptChain C, ConceptResidue R, ConceptAtom A, ConceptBond B>
	class Molecule;

	class Category;
	class Trajectory;

	class SecondaryStructure;
	class SolventExcludedSurface;

} // namespace VTX::Core::NewStruct
