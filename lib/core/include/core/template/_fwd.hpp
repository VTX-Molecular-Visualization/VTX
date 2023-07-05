#include "concept.hpp"

namespace VTX::Core::Template
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

} // namespace VTX::Core::Template
