#ifndef VTX_IO_UTIL_CHEMFILES_HPP
#define VTX_IO_UTIL_CHEMFILES_HPP

#include "io/internal/filesystem.hpp"
#include <core/chemdb/bond.hpp>
#include <util/math/aabb.hpp>
#ifdef _MSC_VER
#pragma warning( push, 0 )
#endif
#include <chemfiles.hpp>
#ifdef _MSC_VER
#pragma warning( pop )
#endif
#include <algorithm>
#include <string>
#include <unordered_set>

namespace VTX::IO::Util::Chemfiles
{
	chemfiles::Bond::BondOrder convertBondOrder( VTX::Core::ChemDB::Bond::ORDER p_bondOrder );

} // namespace VTX::IO::Util::Chemfiles

#endif
