#ifndef __VTX_CORE_TEMPLATE_CONCEPT__
#define __VTX_CORE_TEMPLATE_CONCEPT__

#include <concepts>

namespace VTX::Core::Template
{
	template<typename M>
	concept ConceptMolecule = requires() { true; };

	template<typename C>
	concept ConceptChain = requires() { true; };

	template<typename R>
	concept ConceptResidue = requires() { true; };

	template<typename A>
	concept ConceptAtom = requires() { true; };

	template<typename B>
	concept ConceptBond = requires() { true; };
} // namespace VTX::Core::Template
#endif
