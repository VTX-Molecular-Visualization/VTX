#include "core/old/struct/molecule.hpp"
#include "core/chemdb/color.hpp"
#include "core/old/struct/atom.hpp"
#include "core/old/struct/bond.hpp"
#include "core/old/struct/chain.hpp"
#include "core/old/struct/residue.hpp"
#include <algorithm>
#include <util/color/rgba.hpp>
#include <util/logger.hpp>

namespace VTX::Core::Old::Struct
{
	Molecule::Molecule() {}
	Molecule::~Molecule()
	{
		for ( Atom * atom : _atoms )
			delete atom;
		_atoms.clear();

		for ( Bond * bond : _bonds )
			delete bond;
		_bonds.clear();

		for ( Residue * residue : _residues )
			delete residue;
		_residues.clear();

		for ( Chain * chain : _chains )
			delete chain;
		_chains.clear();

		for ( const ChemDB::UnknownResidueData * const unknownResidueSymbol : _unknownResidueSymbol )
			delete unknownResidueSymbol;
		_unknownResidueSymbol.clear();
	}

	Chain & Molecule::addChain()
	{
		Chain * const chain = new Chain();
		addChain( chain );

		return *chain;
	};
	void Molecule::addChain( Chain * const p_chain )
	{
		_chains.emplace_back( p_chain );
		_realChainCount++;
	}

	Residue & Molecule::addResidue()
	{
		Residue * const residue = new Residue();
		_residues.emplace_back( residue );

		return *residue;
	}

	Atom & Molecule::addAtom()
	{
		Atom * const atom = new Atom();
		_atoms.emplace_back( atom );

		return *atom;
	}

	Bond & Molecule::addBond()
	{
		Bond * const bond = new Bond();
		_bonds.emplace_back( bond );

		return *bond;
	}

	int Molecule::getUnknownResidueSymbolIndex( const std::string & p_symbol ) const
	{
		size_t residueIndex;

		for ( residueIndex = 0; residueIndex < _unknownResidueSymbol.size(); residueIndex++ )
		{
			if ( _unknownResidueSymbol[ residueIndex ]->symbolStr == p_symbol )
				return int( residueIndex );
		}

		return -1;
	}
	ChemDB::UnknownResidueData * const Molecule::getUnknownResidueSymbol( const uint p_unkownymbolIndex ) const
	{
		return _unknownResidueSymbol[ p_unkownymbolIndex ];
	}

	ChemDB::UnknownResidueData * const Molecule::getUnknownResidueSymbol( const std::string & p_symbol ) const
	{
		for ( size_t residueIndex = 0; residueIndex < _unknownResidueSymbol.size(); residueIndex++ )
		{
			if ( _unknownResidueSymbol[ residueIndex ]->symbolStr == p_symbol )
				return _unknownResidueSymbol[ residueIndex ];
		}

		return nullptr;
	}

	int Molecule::addUnknownResidueSymbol( ChemDB::UnknownResidueData * const p_residueData )
	{
		size_t residueIndex;

		for ( residueIndex = 0; residueIndex < _unknownResidueSymbol.size(); residueIndex++ )
		{
			if ( _unknownResidueSymbol[ residueIndex ]->symbolStr == p_residueData->symbolStr )
				return int( residueIndex );
		}

		_unknownResidueSymbol.emplace_back( p_residueData );
		return int( residueIndex );
	}

	bool Molecule::isEmpty()
	{
		for ( uint i = 0; i < getChainCount(); i++ )
		{
			if ( _chains[ i ] != nullptr )
			{
				return false;
			}
		}

		return true;
	}

	bool Molecule::mergeTopology( const Molecule & p_molecule )
	{
		if ( p_molecule.getAtomCount() > getAtomCount() )
		{
			VTX_ERROR( "Too many atoms in topology model" );
			return false;
		}
		else if ( getAtomCount() > p_molecule.getAtomCount() )
		{
			VTX_WARNING( "Topology model has less atoms than dynamic one (maybe solvent not present)" );
		}

		// Clear topology.
		for ( Chain * chain : _chains )
			delete chain;
		_chains.clear();

		for ( Residue * residue : _residues )
			delete residue;
		_residues.clear();

		// Create models.
		_chains.resize( p_molecule.getChainCount() );
		for ( uint i = 0; i < p_molecule.getChainCount(); ++i )
		{
			getChains()[ i ] = new Chain();
		}
		_residues.resize( p_molecule.getResidueCount() );
		for ( uint i = 0; i < p_molecule.getResidueCount(); ++i )
		{
			getResidues()[ i ] = new Residue();
		}

		setName( p_molecule.getName() );
		// Loop over other molecule chains.
		for ( uint chainIndex = 0; chainIndex < p_molecule.getChainCount(); ++chainIndex )
		{
			const Chain & otherChain = *p_molecule.getChain( chainIndex );
			Chain &		  modelChain = *getChain( chainIndex );
			modelChain.setIndex( otherChain.getIndex() );
			modelChain.setMoleculePtr( this );
			modelChain.setName( otherChain.getName() );
			modelChain.setIndexFirstResidue( otherChain.getIndexFirstResidue() );
			modelChain.setResidueCount( otherChain.getResidueCount() );

			for ( uint residueIndex = 0; residueIndex < otherChain.getResidueCount(); ++residueIndex )
			{
				const Residue * const otherResidue
					= p_molecule.getResidue( otherChain.getIndexFirstResidue() + residueIndex );
				Residue * const modelResidue = getResidue( otherChain.getIndexFirstResidue() + residueIndex );

				modelResidue->setIndex( otherResidue->getIndex() );
				modelResidue->setChainPtr( &modelChain );
				modelResidue->setIndexFirstAtom( otherResidue->getIndexFirstAtom() );
				modelResidue->setAtomCount( otherResidue->getAtomCount() );
				modelResidue->setSymbol( otherResidue->getSymbol() );

				for ( uint atomIndex = 0; atomIndex < otherResidue->getAtomCount(); ++atomIndex )
				{
					const Atom & otherAtom = *p_molecule.getAtom( otherResidue->getIndexFirstAtom() + atomIndex );
					Atom &		 modelAtom = *getAtom( otherResidue->getIndexFirstAtom() + atomIndex );
					modelAtom.setIndex( otherAtom.getIndex() );
					modelAtom.setResiduePtr( modelResidue );
				}
			}
		}

		// Loop over other atoms.
		if ( getAtomCount() > p_molecule.getAtomCount() )
		{
			addChain();
			addResidue();
			Chain &	  modelChain   = *getChain( getChainCount() - 1 );
			Residue & modelResidue = *getResidue( getResidueCount() - 1 );
			modelChain.setIndex( getChainCount() - 1 );
			modelChain.setIndexFirstResidue( getResidueCount() - 1 );
			modelChain.setResidueCount( 1 );
			modelResidue.setIndex( getResidueCount() - 1 );
			modelResidue.setChainPtr( &modelChain );
			modelResidue.setIndexFirstAtom( p_molecule.getAtomCount() );
			modelResidue.setAtomCount( getAtomCount() - p_molecule.getAtomCount() );

			for ( uint atomIndex = 0; atomIndex < modelResidue.getAtomCount(); ++atomIndex )
			{
				Atom & modelAtom = *getAtom( modelResidue.getIndexFirstAtom() + atomIndex );
				modelAtom.setResiduePtr( &modelResidue );
			}
		}

		return true;
	}

	Chain * Molecule::getFirstChain()
	{
		for ( Chain * const chain : _chains )
			if ( chain != nullptr )
				return chain;

		return nullptr;
	}
	const Chain * const Molecule::getFirstChain() const
	{
		for ( const Chain * const chain : _chains )
			if ( chain != nullptr )
				return chain;

		return nullptr;
	}
	const Chain * const Molecule::getPreviousChain( const uint p_idBaseChain ) const
	{
		if ( p_idBaseChain == 0 )
			return nullptr;

		for ( uint i = p_idBaseChain - 1; i > 0; i-- )
			if ( _chains[ i ] != nullptr )
				return _chains[ i ];

		return _chains[ 0 ];
	}
	Chain * const Molecule::getPreviousChain( const uint p_idBaseChain )
	{
		if ( p_idBaseChain == 0 )
			return nullptr;

		for ( uint i = p_idBaseChain - 1; i > 0; i-- )
			if ( _chains[ i ] != nullptr )
				return _chains[ i ];

		return _chains[ 0 ];
	}
	const Chain * const Molecule::getNextChain( const uint p_idBaseChain ) const
	{
		for ( uint i = p_idBaseChain + 1; i < _chains.size(); i++ )
			if ( _chains[ i ] != nullptr )
				return _chains[ i ];
		return nullptr;
	}
	Chain * const Molecule::getNextChain( const uint p_idBaseChain )
	{
		for ( uint i = p_idBaseChain + 1; i < _chains.size(); i++ )
			if ( _chains[ i ] != nullptr )
				return _chains[ i ];
		return nullptr;
	}

	const Residue * const Molecule::getPreviousResidue( const uint p_idBaseResidue ) const
	{
		if ( p_idBaseResidue == 0 )
			return nullptr;

		for ( uint i = p_idBaseResidue - 1; i > 0; i-- )
			if ( _residues[ i ] != nullptr )
				return _residues[ i ];

		return _residues[ 0 ];
	}
	Residue * const Molecule::getPreviousResidue( const uint p_idBaseResidue )
	{
		if ( p_idBaseResidue == 0 )
			return nullptr;

		for ( uint i = p_idBaseResidue - 1; i > 0; i-- )
			if ( _residues[ i ] != nullptr )
				return _residues[ i ];

		return _residues[ 0 ];
	}
	const Residue * const Molecule::getNextResidue( const uint p_idBaseResidue ) const
	{
		for ( uint i = p_idBaseResidue + 1; i < _residues.size(); i++ )
			if ( _residues[ i ] != nullptr )
				return _residues[ i ];
		return nullptr;
	}
	Residue * const Molecule::getNextResidue( const uint p_idBaseResidue )
	{
		for ( uint i = p_idBaseResidue + 1; i < _residues.size(); i++ )
			if ( _residues[ i ] != nullptr )
				return _residues[ i ];
		return nullptr;
	}
	int Molecule::getRealResidueCount() const
	{
		int res = 0;

		for ( size_t chainIdx = 0; chainIdx < _chains.size(); chainIdx++ )
		{
			const Chain * const chain = _chains[ chainIdx ];
			if ( chain == nullptr )
				continue;
			res += chain->getRealResidueCount();
		}

		return res;
	}
	int Molecule::getRealAtomCount() const
	{
		int res = 0;

		for ( size_t residueIdx = 0; residueIdx < _residues.size(); residueIdx++ )
		{
			const Residue * const residue = _residues[ residueIdx ];
			if ( residue == nullptr )
				continue;
			res += residue->getRealAtomCount();
		}

		return res;
	}

	void Molecule::removeChain( const uint p_id, const bool p_delete, const bool p_recursive )
	{
		if ( p_recursive )
		{
			// Delete Residues
			for ( uint residueIndex = _chains[ p_id ]->getIndexFirstResidue();
				  residueIndex < ( _chains[ p_id ]->getIndexFirstResidue() + _chains[ p_id ]->getResidueCount() );
				  residueIndex++ )
			{
				if ( _residues[ residueIndex ] != nullptr )
					removeResidue( residueIndex, p_delete, p_recursive, false );
			}
		}

		Chain * const chainToDelete = _chains[ p_id ];

		_chains[ p_id ] = nullptr;
		_realChainCount--;

		// Delete chain
		if ( p_delete )
			delete chainToDelete;
	}

	void Molecule::removeResidue( const uint p_id,
								  const bool p_delete,
								  const bool p_recursive,
								  const bool p_checkParentUpdate )
	{
		// Delete Atoms
		if ( p_recursive )
		{
			for ( uint atomIndex = _residues[ p_id ]->getIndexFirstAtom();
				  atomIndex < _residues[ p_id ]->getIndexFirstAtom() + _residues[ p_id ]->getAtomCount();
				  atomIndex++ )
			{
				if ( _atoms[ atomIndex ] == nullptr )
					continue;

				removeAtom( atomIndex, p_delete, true, false );
			}
		}

		// Register parent before nullify
		Residue * const residueToDelete = _residues[ p_id ];
		Chain * const	parent			= residueToDelete->getChainPtr();

		_residues[ p_id ] = nullptr;

		// Update parent indexes
		if ( p_checkParentUpdate )
		{
			parent->removeToResidue( p_id );

			if ( parent->getResidueCount() == 0 )
				removeChain( parent->getIndex(), p_delete, false );
		}

		if ( p_checkParentUpdate && parent->getResidueCount() == 0 )
		{
			removeChain( parent->getIndex(), p_delete, true );
		}

		// Remove Residue
		if ( p_delete )
			delete residueToDelete;
	}

	void Molecule::removeAtom( const uint p_id,
							   const bool p_delete,
							   const bool p_checkInternalBonds,
							   const bool p_checkParentUpdate )
	{
		Residue * const parent = _atoms[ p_id ]->getResiduePtr();

		// Delete Bonds
		if ( p_checkInternalBonds )
		{
			for ( uint bondIndex = parent->getIndexFirstBond();
				  bondIndex < parent->getIndexFirstBond() + parent->getBondCount();
				  bondIndex++ )
			{
				const Bond * const bond = _bonds[ bondIndex ];

				if ( bond != nullptr )
				{
					const uint bondFirstAtomIndex  = bond->getIndexFirstAtom();
					const uint bondSecondAtomIndex = bond->getIndexSecondAtom();

					if ( bondFirstAtomIndex == p_id || bondSecondAtomIndex == p_id )
					{
						const Atom * const bondFirstAtom  = _atoms[ bondFirstAtomIndex ];
						const Atom * const bondSecondAtom = _atoms[ bondSecondAtomIndex ];

						const Residue * const residueFirstAtomOfBond
							= bondFirstAtom == nullptr ? nullptr : bondFirstAtom->getResiduePtr();

						const Residue * const residueSecondAtomOfBond
							= bondSecondAtom == nullptr ? nullptr : bondSecondAtom->getResiduePtr();

						// If external bond => need to remove the bond of the other residue
						if ( residueFirstAtomOfBond != nullptr && residueSecondAtomOfBond != nullptr
							 && residueFirstAtomOfBond != residueSecondAtomOfBond )
						{
							if ( residueFirstAtomOfBond == parent )
							{
								const uint otherBondIndex
									= residueSecondAtomOfBond->findBondIndex( bondFirstAtomIndex, bondSecondAtomIndex );

								removeBond( bondIndex, p_delete );
								removeBond( otherBondIndex, p_delete );
							}
							else
							{
								const uint otherBondIndex
									= residueFirstAtomOfBond->findBondIndex( bondFirstAtomIndex, bondSecondAtomIndex );

								removeBond( bondIndex, p_delete );
								removeBond( otherBondIndex, p_delete );
							}
						}
						else
						{
							removeBond( bondIndex, p_delete );
						}
					}
				}
			}
		}

		Atom * const atomToDelete = _atoms[ p_id ];

		_atoms[ p_id ] = nullptr;

		// Update parent indexes
		if ( p_checkParentUpdate )
		{
			parent->removeToAtom( p_id );
		}

		if ( p_checkParentUpdate && parent->getAtomCount() == 0 )
		{
			removeResidue( parent->getIndex(), p_delete, true, p_checkParentUpdate );
		}

		// Delete Atom
		if ( p_delete )
			delete atomToDelete;
	}

	void Molecule::removeBond( const uint p_id, const bool p_delete )
	{
		if ( p_delete )
			delete _bonds[ p_id ];

		_bonds[ p_id ] = nullptr;
	}
} // namespace VTX::Core::Old::Struct
