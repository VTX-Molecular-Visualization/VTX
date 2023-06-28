#ifndef __VTX_APP_OLD_INTERNAL_CHEMFILES_UTIL__
#define __VTX_APP_OLD_INTERNAL_CHEMFILES_UTIL__

#include "app/old/component/object3d/helper/aabb.hpp"
#include "app/old/internal/io/filesystem.hpp"
#include <core/chemdb/bond.hpp>
#pragma warning( push, 0 )
#include <chemfiles.hpp>
#pragma warning( pop )
#include <algorithm>
#include <string>
#include <unordered_set>

namespace VTX::App::Old::Internal::Chemfiles::Util
{
	chemfiles::Bond::BondOrder convertBondOrder( const VTX::Core::ChemDB::Bond::ORDER p_bondOrder );

	void recomputeBonds( chemfiles::Frame & p_frame, const Component::Object3D::Helper::AABB & p_aabb );
	void recomputeBondOrders( chemfiles::Frame & p_frame );
	bool recomputeBondOrdersFromFile( chemfiles::Frame & p_frame );

} // namespace VTX::App::Old::Internal::Chemfiles::Util

#endif
