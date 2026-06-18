#ifndef __VTX_IO_UTIL_SECONDARY_STRUCTURE__
#define __VTX_IO_UTIL_SECONDARY_STRUCTURE__

#include <core/struct/topology.hpp>
#include <core/struct/trajectory.hpp>

namespace VTX::IO::Util::SecondaryStructure
{

	/**
	 * @brief Assign secondary structure types to residues in a topology based on the provided frame.
	 */
	void assignSecondaryStructure( VTX::Core::Struct::Topology &, const VTX::Core::Struct::Frame & );

} // namespace VTX::IO::Util::SecondaryStructure

#endif
