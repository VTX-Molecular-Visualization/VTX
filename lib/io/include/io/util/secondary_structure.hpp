#ifndef __VTX_IO_UTIL_SECONDARY_STRUCTURE__
#define __VTX_IO_UTIL_SECONDARY_STRUCTURE__

#include <core/struct/topology.hpp>
#include <core/struct/trajectory.hpp>

namespace VTX::IO::Util::SecondaryStructure
{

	void assignSecondaryStructure( VTX::Core::Struct::Topology &, const VTX::Core::Struct::Frame & );

} // namespace VTX::IO::Util::SecondaryStructure

#endif
