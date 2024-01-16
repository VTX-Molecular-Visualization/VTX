#ifndef __VTX_IO_UTIL_SECONDARY_STRUCTURE__
#define __VTX_IO_UTIL_SECONDARY_STRUCTURE__

#include <core/struct/molecule.hpp>

namespace VTX::IO::Util::SecondaryStructure
{

	void computeStride( Core::Struct::Molecule & p_molecule );

} // namespace VTX::IO::Util::SecondaryStructure

#endif
