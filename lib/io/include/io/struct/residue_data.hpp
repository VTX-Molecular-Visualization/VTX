#ifndef __VTX_IO_STRUCT_RESIDUE_DATA__
#define __VTX_IO_STRUCT_RESIDUE_DATA__

#include "io/struct/bond_data.hpp"
#include <core/chemdb/category.hpp>
#include <string>
#include <vector>

namespace VTX::IO::Struct
{
	struct ResidueData
	{
	  public:
		static const ResidueData DEFAULT;

		ResidueData() {};
		ResidueData( const std::string & p_fullname, const std::vector<BondData> & p_bondData ) :
			fullname( p_fullname ), bondData( p_bondData ) {};

		std::string						  fullname = "";
		VTX::Core::ChemDB::Category::TYPE category = VTX::Core::ChemDB::Category::TYPE::UNKNOWN;
		std::vector<BondData>			  bondData = std::vector<BondData>();
	};
} // namespace VTX::IO::Struct
#endif
