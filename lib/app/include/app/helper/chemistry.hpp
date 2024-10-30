#ifndef __VTX_APP_HELPER_CHEMISTRY__
#define __VTX_APP_HELPER_CHEMISTRY__

#include "app/component/chemistry/_fwd.hpp"
#include <io/struct/bond_data.hpp>
#include <io/struct/residue_data.hpp>
#include <map>
#include <string>
#include <vector>

namespace VTX::App::Helper::Chemistry
{

	std::vector<Component::Chemistry::Chain *> findChainsByName(
		const Component::Chemistry::System & p_system,
		const std::string &					   p_chainName
	);
	std::vector<Component::Chemistry::Residue *> findResiduesByName(
		const Component::Chemistry::Chain & p_system,
		const std::string &					p_residueName
	);

	inline std::map<std::string, IO::Struct::ResidueData> mapLoadedResidueData
		= { { "", IO::Struct::ResidueData::DEFAULT } };

	void									  loadResidueData( const std::string & p_residueSymbol );
	const std::string &						  getResidueFullName( const std::string & p_residueSymbol );
	const std::vector<IO::Struct::BondData> & getResidueBonds( const std::string & p_residueSymbol );
	VTX::Core::ChemDB::Category::TYPE		  getResidueCategory( const std::string & p_residueSymbol );
} // namespace VTX::App::Helper::Chemistry

#endif
