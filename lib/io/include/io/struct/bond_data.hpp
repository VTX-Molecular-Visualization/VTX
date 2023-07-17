#ifndef __VTX_IO_STRUCT_BOND_DATA__
#define __VTX_IO_STRUCT_BOND_DATA__

#include <core/chemdb/bond.hpp>
#include <string>

namespace VTX::IO::Struct
{
	struct BondData
	{
	  public:
		std::string					   atom1;
		std::string					   atom2;
		VTX::Core::ChemDB::Bond::ORDER bondOrder;
	};
} // namespace VTX::IO::Struct
#endif
