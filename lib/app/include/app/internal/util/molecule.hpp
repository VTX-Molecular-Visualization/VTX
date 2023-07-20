#ifndef __VTX_INTERNAL_UTIL_MOLECULE__
#define __VTX_INTERNAL_UTIL_MOLECULE__

#include "app/component/chemistry/_fwd.hpp"
#include <core/chemdb/category.hpp>
#include <io/reader/residue_data_reader.hpp>
#include <io/struct/bond_data.hpp>
#include <io/struct/residue_data.hpp>
#include <map>
#include <string>
#include <vector>

namespace VTX::App::Internal::Util::Molecule
{
	inline std::map<std::string, VTX::IO::Struct::ResidueData> mapLoadedResidueData
		= { { "", VTX::IO::Struct::ResidueData::DEFAULT } };

	void										   loadResidueData( const std::string & p_residueSymbol );
	const std::string &							   getResidueFullName( const std::string & p_residueSymbol );
	const std::vector<VTX::IO::Struct::BondData> & getResidueBonds( const std::string & p_residueSymbol );
	VTX::Core::ChemDB::Category::TYPE			   getResidueCategory( const std::string & p_residueSymbol );

} // namespace VTX::App::Internal::Util::Molecule

#endif
