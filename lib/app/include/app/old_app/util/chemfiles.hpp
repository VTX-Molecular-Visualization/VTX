#ifndef __VTX_UTIL_CHEMFILES__
#define __VTX_UTIL_CHEMFILES__

#include "app/old_app/model/bond.hpp"
#include "app/old_app/object3d/helper/aabb.hpp"
#include "app/old_app/io/filesystem.hpp"
#pragma warning( push, 0 )
#include <chemfiles.hpp>
#pragma warning( pop )
#include <algorithm>
#include <string>
#include <unordered_set>

namespace VTX::Util::Chemfiles
{
	chemfiles::Bond::BondOrder convertBondOrder( const Model::Bond::ORDER p_bondOrder );

	void recomputeBonds( chemfiles::Frame & p_frame, const VTX::Object3D::Helper::AABB & p_aabb );
	void recomputeBondOrders( chemfiles::Frame & p_frame );
	bool recomputeBondOrdersFromFile( chemfiles::Frame & p_frame );

} // namespace VTX::Util::Chemfiles

#endif