#ifndef __VTX_IO_UTIL_BOND_ORDER_GUESSING__
#define __VTX_IO_UTIL_BOND_ORDER_GUESSING__

#include <core/struct/topology.hpp>
#include <core/struct/trajectory.hpp>

namespace VTX::IO::Util
{
	/**
	 * @brief Recompute bond orders for a given topology and frame.
	 */
	void recomputeBondOrders( VTX::Core::Struct::Topology & p_topology, const VTX::Core::Struct::Frame & p_frame );

} // namespace VTX::IO::Util

#endif
