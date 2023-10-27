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
} // namespace VTX::App::Helper::Chemistry
