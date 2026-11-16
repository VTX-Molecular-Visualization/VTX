#ifndef VTX_APP_HELPER_CHEMISTRY_HPP
#define VTX_APP_HELPER_CHEMISTRY_HPP

#include <core/struct/topology.hpp>
#include <string>
#include <vector>

namespace VTX::App::Helper::Chemistry
{
	std::vector<Index> findChainsByName( const Core::Struct::Topology & p_topology, const std::string & p_chainName );
	std::vector<Index> findResiduesByName( const Core::Struct::Topology & p_topology,
										   const std::string &			  p_residueName );
} // namespace VTX::App::Helper::Chemistry

#endif
