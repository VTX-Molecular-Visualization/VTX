#ifndef __VTX_IO_UTIL_CHEMFILES__
#define __VTX_IO_UTIL_CHEMFILES__

#include "io/internal/filesystem.hpp"
#include <core/chemdb/bond.hpp>
#include <util/math/aabb.hpp>
#pragma warning( push, 0 )
#include <chemfiles.hpp>
#pragma warning( pop )
#include <algorithm>
#include <string>
#include <unordered_set>

namespace VTX::IO::Util::Chemfiles
{
	chemfiles::Bond::BondOrder convertBondOrder( const VTX::Core::ChemDB::Bond::ORDER p_bondOrder );

} // namespace VTX::IO::Util::Chemfiles

#endif
