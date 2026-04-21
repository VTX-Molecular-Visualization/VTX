#ifndef __VTX_APP_HELPER_CHEMISTRY__
#define __VTX_APP_HELPER_CHEMISTRY__

#include <core/struct/topology.hpp>
#include <io/struct/bond_data.hpp>
#include <io/struct/residue_data.hpp>
#include <map>
#include <string>
#include <vector>

namespace VTX::App::Helper::Chemistry
{
	std::vector<Index> findChainsByName( const Core::Struct::Topology & p_topology, const std::string & p_chainName );
	std::vector<Index> findResiduesByName(
		const Core::Struct::Topology & p_topology,
		const std::string &			   p_residueName
	);
} // namespace VTX::App::Helper::Chemistry

#endif
