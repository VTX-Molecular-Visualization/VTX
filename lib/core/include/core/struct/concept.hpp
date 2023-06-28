#ifndef __VTX_CORE_NEW_STRUCT_CONCEPT__
#define __VTX_CORE_NEW_STRUCT_CONCEPT__

#include <concepts>

namespace VTX::Core::Struct
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
} // namespace VTX::Core::Struct
#endif
