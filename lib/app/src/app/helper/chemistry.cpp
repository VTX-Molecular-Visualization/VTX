#include "app/helper/chemistry.hpp"

namespace VTX::App::Helper::Chemistry
{
	std::vector<Index> findChainsByName( const Core::Struct::Topology & p_topology, const std::string & p_chainName )
	{
		std::vector<Index> res;

		for ( Index i = 0; i < p_topology.getChainCount(); ++i )
		{
			if ( p_topology.chainNames[ i ] == p_chainName )
			{
				res.emplace_back( i );
			}
		}

		return res;
	}

	std::vector<Index> findResiduesByName( const Core::Struct::Topology & p_topology,
										   const std::string &			  p_residueName )
	{
		using namespace VTX::Core;

		std::vector<Index>			  res;
		const ChemDB::Residue::SYMBOL residueSymbol = ChemDB::Residue::getSymbolFromAnyName( p_residueName );

		if ( residueSymbol != ChemDB::Residue::SYMBOL::UNKNOWN )
		{
			for ( Index iRes = 0; iRes < p_topology.getResidueCount(); ++iRes )
			{
				if ( p_topology.getResidueSymbol( iRes ) == residueSymbol )
				{
					res.emplace_back( iRes );
				}
			}
		}
		else // Unknown symbol => check names
		{
			for ( Index iRes = 0; iRes < p_topology.getResidueCount(); ++iRes )
			{
				if ( p_topology.getResidueSymbol( iRes ) != ChemDB::Residue::SYMBOL::UNKNOWN )
				{
					continue;
				}

				if ( p_topology.getResidueName( iRes ) == p_residueName )
				{
					res.emplace_back( iRes );
				}
			}
		}

		return res;
	}

} // namespace VTX::App::Helper::Chemistry
