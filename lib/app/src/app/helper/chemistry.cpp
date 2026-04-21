#include "app/helper/chemistry.hpp"
#include "app/services.hpp"
#include "app/session.hpp"
#include <io/reader/residue_data_reader.hpp>

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

	std::vector<Index> findResiduesByName(
		const Core::Struct::Topology & p_topology,
		const std::string &			   p_residueName
	)
	{
		using namespace VTX::Core;

		std::vector<Index>			  res;
		const ChemDB::Residue::SYMBOL residueSymbol = ChemDB::Residue::getSymbolFromAnyName( p_residueName );

		/*
		if ( residueSymbol != ChemDB::Residue::SYMBOL::UNKNOWN )
		{
			for ( Index iRes = p_chain.getIndexFirstResidue(); iRes <= p_chain.getIndexLastResidue(); iRes++ )
			{
				Component::Chemistry::Residue * const residue = system.getResidue( iRes );

				if ( residue != nullptr && residue->getSymbol() == residueSymbol )
					res.emplace_back( residue );
			}
		}
		else // Unknown symbol => check names
		{
			for ( Index iRes = p_chain.getIndexFirstResidue(); iRes <= p_chain.getIndexLastResidue(); iRes++ )
			{
				Component::Chemistry::Residue * const residue = system.getResidue( iRes );

				if ( residue == nullptr || residue->getSymbol() != ChemDB::Residue::SYMBOL::UNKNOWN )
					continue;

				if ( residue->getName() == p_residueName || residue->getShortName() == p_residueName
					 || residue->getLongName() == p_residueName )
				{
					res.emplace_back( residue );
				}
			}
		}
		*/

		return res;
	}

} // namespace VTX::App::Helper::Chemistry
