#ifndef __VTX_APP_UTIL_CHEMFILES__
#define __VTX_APP_UTIL_CHEMFILES__

#include "app/component/object3d/helper/aabb.hpp"
#include "app/internal/chemdb/bond.hpp"
#include "app/internal/io/filesystem.hpp"
#pragma warning( push, 0 )
#include <chemfiles.hpp>
#pragma warning( pop )
#include <algorithm>
#include <string>
#include <unordered_set>

namespace VTX::Util::App::Chemfiles
{
	chemfiles::Bond::BondOrder convertBondOrder( const VTX::App::Internal::ChemDB::Bond::ORDER p_bondOrder );

	void recomputeBonds( chemfiles::Frame & p_frame, const VTX::App::Component::Object3D::Helper::AABB & p_aabb );
	void recomputeBondOrders( chemfiles::Frame & p_frame );
	bool recomputeBondOrdersFromFile( chemfiles::Frame & p_frame );

} // namespace VTX::Util::App::Chemfiles

#endif
