#include "app/application/selection/molecule_data.hpp"
#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/component/scene/selectable.hpp"
#include "app/core/ecs/registry.hpp"
#include "app/vtx_app.hpp"
#include <sstream>

namespace VTX::App::Application::Selection
{
	MoleculeData::MoleculeData( const Component::Scene::Selectable & p_selectable ) :
		SelectionData( p_selectable ),
		_molecule( &VTXApp::get().MAIN_REGISTRY().getComponent<Molecule>( p_selectable ) )
	{
	}

	bool MoleculeData::isEqualsTo( const SelectionData & p_other ) const
	{
		const MoleculeData & castedOther = dynamic_cast<const MoleculeData &>( p_other );

		return _molecule == castedOther._molecule && _chainIds == castedOther._chainIds
			   && _residueIds == castedOther._residueIds && _atomIds == castedOther._atomIds;
	}

	std::unique_ptr<SelectionData> MoleculeData::_cloneImpl() const
	{
		std::unique_ptr<MoleculeData> copy = std::make_unique<MoleculeData>( getSelectionComponent() );

		copy->_chainIds	  = _chainIds;
		copy->_residueIds = _residueIds;
		copy->_atomIds	  = _atomIds;

		return std::move( copy );
	}

	SelectionData & MoleculeData::add( const SelectionData & p_other )
	{
		const MoleculeData & castedOther = dynamic_cast<const MoleculeData &>( p_other );

		_chainIds.merge( castedOther._chainIds );
		_residueIds.merge( castedOther._residueIds );
		_atomIds.merge( castedOther._atomIds );

		return *this;
	}
	SelectionData & MoleculeData::remove( const SelectionData & p_other )
	{
		const MoleculeData & castedOther = dynamic_cast<const MoleculeData &>( p_other );

		_chainIds.substract( castedOther._chainIds );
		_residueIds.substract( castedOther._residueIds );
		_atomIds.substract( castedOther._atomIds );

		_valid = _chainIds.size() > 0 || _residueIds.size() > 0 || _atomIds.size() > 0;

		return *this;
	}
	SelectionData & MoleculeData::intersect( const SelectionData & p_other ) { return *this; }
	SelectionData & MoleculeData::exclude( const SelectionData & p_other ) { return *this; }

	void MoleculeData::selectAll()
	{
		_chainIds.addRange( IndexRange( 0, _molecule->getChains().size() ) );
		_residueIds.addRange( IndexRange( 0, _molecule->getResidues().size() ) );
		_atomIds.addRange( IndexRange( 0, _molecule->getAtoms().size() ) );
	}

	// Chains ////////////////////////////////////////////////////////////////////////////////////////
	void MoleculeData::referenceChain( const Chain & p_chain ) { _referenceChain( p_chain ); }
	void MoleculeData::selectFullChain( const Chain & p_chain ) { _selectFullChain( p_chain ); }
	void MoleculeData::referenceChains( const std::vector<Chain *> & p_chains )
	{
		for ( const Chain * const chainPtr : p_chains )
		{
			if ( chainPtr != nullptr )
				_referenceChain( *chainPtr );
		}
	}
	void MoleculeData::selectFullChains( const std::vector<Chain *> & p_chains )
	{
		for ( const Chain * const chainPtr : p_chains )
		{
			if ( chainPtr != nullptr )
				_selectFullChain( *chainPtr );
		}
	}
	void MoleculeData::referenceChains( const IndexRange & p_range ) { _referenceChains( p_range ); }
	void MoleculeData::selectFullChains( const IndexRange & p_range ) { _selectFullChains( p_range ); }

	void MoleculeData::referenceChains( const IndexRangeList & p_rangeList )
	{
		for ( const size_t index : p_rangeList )
		{
			const Chain * const chainPtr = _molecule->getChain( index );

			if ( chainPtr != nullptr )
				_referenceChain( *chainPtr );
		}
	}
	void MoleculeData::selectFullChains( const IndexRangeList & p_rangeList )
	{
		for ( const size_t index : p_rangeList )
		{
			const Chain * const chainPtr = _molecule->getChain( index );

			if ( chainPtr != nullptr )
				_selectFullChain( *chainPtr );
		}
	}

	// Residues //////////////////////////////////////////////////////////////////////////////////////
	void MoleculeData::referenceResidue( const Residue & p_residue )
	{
		_referenceChain( *p_residue.getChainPtr() );
		_referenceResidue( p_residue );
	}
	void MoleculeData::selectFullResidue( const Residue & p_residue )
	{
		_referenceChain( *p_residue.getChainPtr() );
		_selectFullResidue( p_residue );
	}
	void MoleculeData::referenceResidues( const std::vector<Residue *> & p_residues )
	{
		for ( const Residue * const residuePtr : p_residues )
		{
			if ( residuePtr != nullptr )
			{
				_referenceChain( *residuePtr->getChainPtr() );
				_referenceResidue( *residuePtr );
			}
		}
	}
	void MoleculeData::selectFullResidues( const std::vector<Residue *> & p_residues )
	{
		for ( const Residue * const residuePtr : p_residues )
		{
			if ( residuePtr != nullptr )
			{
				_referenceChain( *residuePtr->getChainPtr() );
				_selectFullResidue( *residuePtr );
			}
		}
	}
	void MoleculeData::referenceResidues( const IndexRange & p_range )
	{
		const size_t firstChainIndex = _molecule->getResidue( p_range.getFirst() )->getChainPtr()->getIndex();
		const size_t lastChainIndex	 = _molecule->getResidue( p_range.getLast() )->getChainPtr()->getIndex();

		_referenceChains( IndexRange::createFirstLast( firstChainIndex, lastChainIndex ) );
		_referenceResidues( p_range );
	}
	void MoleculeData::selectFullResidues( const IndexRange & p_range ) { _selectFullResidues( p_range ); }
	void MoleculeData::referenceResidues( const IndexRangeList & p_rangeList )
	{
		for ( const size_t index : p_rangeList )
		{
			const Residue * const residuePtr = _molecule->getResidue( index );
			_referenceChain( *residuePtr->getChainPtr() );

			if ( residuePtr != nullptr )
				_referenceResidue( *residuePtr );
		}
	}
	void MoleculeData::selectFullResidues( const IndexRangeList & p_rangeList )
	{
		for ( const size_t index : p_rangeList )
		{
			const Residue * const residuePtr = _molecule->getResidue( index );
			_referenceChain( *residuePtr->getChainPtr() );

			if ( residuePtr != nullptr )
				_selectFullResidue( *residuePtr );
		}
	}

	// Atoms /////////////////////////////////////////////////////////////////////////////////////////
	void MoleculeData::selectAtom( const Atom & p_atom ) { _referenceAtom( p_atom ); }
	void MoleculeData::selectAtoms( const std::vector<Atom *> & p_atoms )
	{
		for ( const Atom * const atomPtr : p_atoms )
		{
			if ( atomPtr != nullptr )
				_referenceAtom( *atomPtr );
		}
	}
	void MoleculeData::selectAtoms( const IndexRange & p_range ) { _referenceAtoms( p_range ); }
	void MoleculeData::selectAtoms( const IndexRangeList & p_rangeList )
	{
		for ( const size_t index : p_rangeList )
		{
			const Atom * const atomPtr = _molecule->getAtom( index );

			if ( atomPtr != nullptr )
				_referenceAtom( *atomPtr );
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////

	void MoleculeData::_referenceChain( const Chain & p_chain ) { _chainIds.addValue( p_chain.getIndex() ); }
	void MoleculeData::_selectFullChain( const Chain & p_chain )
	{
		_referenceChain( p_chain );

		const size_t firstResidueIndex = p_chain.getIndexFirstResidue();
		const size_t residueCount	   = p_chain.getResidueCount();

		_selectFullResidues( IndexRange( firstResidueIndex, residueCount ) );
	}
	void MoleculeData::_referenceChains( const IndexRange & p_range ) { _chainIds.addRange( p_range ); }
	void MoleculeData::_selectFullChains( const IndexRange & p_range )
	{
		_referenceChains( p_range );

		const Chain & firstChain = *_molecule->getChain( p_range.getFirst() );
		const Chain & lastChain	 = *_molecule->getChain( p_range.getLast() );

		const size_t firstResidueIndex = firstChain.getIndexFirstResidue();
		const size_t lastResidueIndex  = lastChain.getIndexFirstResidue() + lastChain.getResidueCount() - 1;

		_selectFullResidues( IndexRange::createFirstLast( firstResidueIndex, lastResidueIndex ) );
	}

	void MoleculeData::_referenceResidue( const Residue & p_residue ) { _residueIds.addValue( p_residue.getIndex() ); }
	void MoleculeData::_selectFullResidue( const Residue & p_residue )
	{
		_referenceResidue( p_residue );

		const size_t firstAtomIndex = p_residue.getIndexFirstAtom();
		const size_t atomCount		= p_residue.getAtomCount();

		_referenceAtoms( IndexRange( firstAtomIndex, atomCount ) );
	}
	void MoleculeData::_referenceResidues( const IndexRange & p_range ) { _residueIds.addRange( p_range ); }
	void MoleculeData::_selectFullResidues( const IndexRange & p_range )
	{
		_referenceResidues( p_range );

		const Residue * const firstResidue = _molecule->getResidue( p_range.getFirst() );
		const Residue * const lastResidue  = _molecule->getResidue( p_range.getLast() );

		const size_t firstAtomIndex = firstResidue->getIndexFirstAtom();
		const size_t lastAtomIndex	= lastResidue->getIndexFirstAtom() + lastResidue->getAtomCount() - 1;

		_referenceAtoms( IndexRange( firstAtomIndex, lastAtomIndex ) );
	}

	void MoleculeData::_referenceAtom( const Atom & p_atom ) { _atomIds.addValue( p_atom.getIndex() ); }
	void MoleculeData::_referenceAtoms( const IndexRange & p_range ) { _atomIds.addRange( p_range ); }

	std::string MoleculeData::toString() const
	{
		std::stringstream sStr			   = std::stringstream();
		const int		  maxItemDisplayed = 20;

		sStr << SelectionData::toString() << std::endl;
		sStr << "Chains  (" << _chainIds.size() << '/' << _molecule->getChains().size() << ")" << std::endl;

		int counter = 0;

		for ( size_t chainId : _chainIds )
		{
			const Chain & chain = *_molecule->getChain( chainId );
			sStr << chain.getName() << '(' << chainId << "),";

			counter++;

			if ( counter >= maxItemDisplayed )
			{
				sStr << "...";
				break;
			}
		}
		sStr << std::endl << std::endl;
		counter = 0;

		sStr << "Residues  (" << _residueIds.size() << '/' << _molecule->getResidues().size() << ")" << std::endl;
		for ( size_t residueId : _residueIds )
		{
			const Residue & residue = *_molecule->getResidue( residueId );
			sStr << residue.getName() << '(' << residueId << "),";

			counter++;

			if ( counter >= maxItemDisplayed )
			{
				sStr << "...";
				break;
			}
		}
		sStr << std::endl << std::endl;
		counter = 0;

		sStr << "Atoms  (" << _atomIds.size() << '/' << _molecule->getAtoms().size() << ")" << std::endl;
		for ( size_t atomId : _atomIds )
		{
			const Atom & atom = *_molecule->getAtom( atomId );
			sStr << atom.getName() << '(' << atomId << "),";

			counter++;

			if ( counter >= maxItemDisplayed )
			{
				sStr << "...";
				break;
			}
		}
		sStr << std::endl;

		return sStr.str();
	}
} // namespace VTX::App::Application::Selection
