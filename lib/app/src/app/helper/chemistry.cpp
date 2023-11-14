#include "app/helper/chemistry.hpp"
#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/bond.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/residue.hpp"

namespace VTX::App::Helper::Chemistry
{
	std::vector<Chain *> findChainsByName( const Molecule & p_molecule, const std::string & p_chainName )
	{
		std::vector<Chain *> res = std::vector<Chain *>();

		for ( Chain * const chain : p_molecule.getChains() )
		{
			if ( chain != nullptr && chain->getName() == p_chainName )
				res.emplace_back( chain );
		}

		return res;
	}

	std::vector<Residue *> findResiduesByName( const Chain & p_chain, const std::string & p_residueName )
	{
		std::vector<Residue *> res		= std::vector<Residue *>();
		Molecule &			   molecule = *( p_chain.getMoleculePtr() );

		const ChemDB::Residue::SYMBOL residueSymbol = ChemDB::Residue::getSymbolFromAnyName( p_residueName );

		if ( residueSymbol != ChemDB::Residue::SYMBOL::UNKNOWN )
		{
			for ( size_t iRes = p_chain.getIndexFirstResidue(); iRes <= p_chain.getIndexLastResidue(); iRes++ )
			{
				Residue * const residue = molecule.getResidue( iRes );

				if ( residue != nullptr && residue->getSymbol() == residueSymbol )
					res.emplace_back( residue );
			}
		}
		else // Unknown symbol => check names
		{
			for ( size_t iRes = p_chain.getIndexFirstResidue(); iRes <= p_chain.getIndexLastResidue(); iRes++ )
			{
				Residue * const residue = molecule.getResidue( iRes );

				if ( residue == nullptr || residue->getSymbol() != ChemDB::Residue::SYMBOL::UNKNOWN )
					continue;

				if ( residue->getName() == p_residueName || residue->getShortName() == p_residueName
					 || residue->getLongName() == p_residueName )
				{
					res.emplace_back( residue );
				}
			}
		}

		return res;
	}

} // namespace VTX::App::Helper::Chemistry
