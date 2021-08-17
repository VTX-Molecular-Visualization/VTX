#ifndef __VTX_UTIL_CHEMFILES__
#define __VTX_UTIL_CHEMFILES__

#include "math/aabb.hpp"
#include "model/bond.hpp"
#include "util/filesystem.hpp"
#pragma warning( push, 0 )
#include <chemfiles.hpp>
#pragma warning( pop )
#include <algorithm>
#include <string>
#include <unordered_set>

namespace VTX::Util::Chemfiles
{
	chemfiles::Bond::BondOrder convertBondOrder( const Model::Bond::ORDER p_bondOrder );

	void recomputeBonds( chemfiles::Frame & p_frame, const VTX::Math::AABB & p_aabb );
	void recomputeBondOrders( chemfiles::Frame & p_frame );
	void recomputeBondOrdersFromFile( chemfiles::Frame & p_frame );

} // namespace VTX::Util::Chemfiles

#endif
