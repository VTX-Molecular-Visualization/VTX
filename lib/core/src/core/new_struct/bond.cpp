#include "core/new_struct/bond.hpp"
#include "core/new_struct/molecule.hpp"

namespace VTX::Core::Struct
{
	size_t Bond::getIndexFirstAtom() const { return moleculePtr->gpuStruct.bonds[ internalIndex * 2 ]; }
	void   Bond::setIndexFirstAtom( const size_t p_atomIndex )
	{
		moleculePtr->gpuStruct.bonds[ internalIndex * 2 ] = p_atomIndex;
	}
	size_t Bond::getIndexSecondAtom() const { return moleculePtr->gpuStruct.bonds[ internalIndex * 2 + 1 ]; }
	void   Bond::setIndexSecondAtom( const size_t p_atomIndex )
	{
		moleculePtr->gpuStruct.bonds[ internalIndex * 2 + 1 ] = p_atomIndex;
	}
} // namespace VTX::Core::Struct
