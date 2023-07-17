#include "io/struct/residue_data.hpp"

namespace VTX::IO::Struct
{
	const ResidueData ResidueData::DEFAULT = ResidueData( "Unknown", std::vector<BondData>() );
} // namespace VTX::IO::Struct
