#include "app/application/selection/molecule_data.hpp"
#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/component/scene/selectable.hpp"
#include "app/core/ecs/registry.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Application::Selection
{
	MoleculeData::MoleculeData( Component::Scene::Selectable & p_selectable ) :
		SelectionData( p_selectable ),
		_molecule( &VTXApp::get().MAIN_REGISTRY().getComponent<Component::Chemistry::Molecule>( p_selectable ) )
	{
	}

	void MoleculeData::selectAll()
	{
		_chainIds.addRange( IndexRange( 0, _molecule->getChains().size() ) );
		_residueIds.addRange( IndexRange( 0, _molecule->getResidues().size() ) );
		_atomIds.addRange( IndexRange( 0, _molecule->getAtoms().size() ) );
	}

	void MoleculeData::selectChain( const Component::Chemistry::Chain & p_chain )
	{
		_chainIds.addValue( p_chain.getIndex() );

		const size_t firstResidueIndex = p_chain.getIndexFirstResidue();
		const size_t residueCount	   = p_chain.getResidueCount();
		_residueIds.addRange( IndexRange( firstResidueIndex, residueCount ) );

		const Component::Chemistry::Residue * const firstResidue   = _molecule->getResidue( firstResidueIndex );
		size_t										firstAtomIndex = firstResidue->getIndexFirstAtom();

		const Component::Chemistry::Residue * const lastResidue
			= _molecule->getResidue( firstResidueIndex + residueCount - 1 );

		const size_t lastAtomIndex = lastResidue->getIndexFirstAtom() + lastResidue->getAtomCount() - 1;
		_residueIds.addRange( IndexRange( firstAtomIndex, lastAtomIndex ) );
	}
	void MoleculeData::selectChains( const std::vector<Component::Chemistry::Chain *> p_chains )
	{
		for ( Component::Chemistry::Chain * chainPtr : p_chains )
		{
			if ( chainPtr != nullptr )
				selectChain( *chainPtr );
		}
	}
	void MoleculeData::selectChains( const IndexRange p_range ) { _chainIds.addRange( p_range ); }
} // namespace VTX::App::Application::Selection
