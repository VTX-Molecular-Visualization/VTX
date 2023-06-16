#ifndef __VTX_CORE_NEW_STRUCT_RESIDUE__
#define __VTX_CORE_NEW_STRUCT_RESIDUE__

#include "_fwd.hpp"
#include "core/chemdb/residue.hpp"
#include "core/define.hpp"
#include "define.hpp"

namespace VTX::Core::NewStruct
{
	template<ConceptMolecule M, ConceptChain C>
	class Residue
	{
	  public:
		Residue() = default;
		Residue( const size_t p_internalIndex ) : _internalIndex( p_internalIndex ) {};
		Residue( C * const p_chain ) : _chainPtr( p_chain ) {};
		Residue( C * const p_chain, const size_t p_internalIndex ) :
			_chainPtr( p_chain ), _internalIndex( p_internalIndex ) {};

		const M * const getConstMoleculePtr() const { return _chainPtr->getConstMoleculePtr(); }
		M * const		getMoleculePtr() const { return _chainPtr->getMoleculePtr(); }
		const C * const getConstChainPtr() const { return _chainPtr; }
		C * const		getChainPtr() const { return _chainPtr; }
		void			setChainPtr( C * const p_chainPtr ) { _chainPtr = p_chainPtr; }

		size_t getIndex() const { return _internalIndex; }
		void   setIndex( const size_t p_index ) { _internalIndex = p_index; }

		const size_t getIndexFirstAtom() const { return _indexFirstAtom; };
		void		 setIndexFirstAtom( const size_t p_indexFirstAtom ) { _indexFirstAtom = p_indexFirstAtom; };
		const size_t getAtomCount() const { return _atomCount; };
		void		 setAtomCount( const size_t p_atomCount ) { _atomCount = p_atomCount; };

		const size_t getIndexFirstBond() const { return _indexFirstBond; };
		void		 setIndexFirstBond( const size_t p_indexFirstBond ) { _indexFirstBond = p_indexFirstBond; };
		const size_t getBondCount() const { return _bondCount; };
		void		 setBondCount( const size_t p_bondCount ) { _bondCount = p_bondCount; };

		ChemDB::Residue::SYMBOL getSymbol() const { return _symbol; }
		void					setSymbol( const ChemDB::Residue::SYMBOL p_symbol ) { _symbol = p_symbol; }

	  protected:
		C *	   _chainPtr	  = nullptr;
		size_t _internalIndex = INVALID_ID;

		ChemDB::Residue::SYMBOL _symbol			= ChemDB::Residue::SYMBOL::UNKNOWN;
		size_t					_indexFirstAtom = INVALID_ID;
		size_t					_atomCount		= 0;
		size_t					_indexFirstBond = INVALID_ID;
		size_t					_bondCount		= 0;

		// ChemDB::Residue::TYPE _type					= ChemDB::Residue::TYPE::STANDARD;
		// int _indexInOriginalChain = 0;
		// uint			   _realAtomCount  = 0;
		// ChemDB::Atom::TYPE _atomType	   = ChemDB::Atom::TYPE::NORMAL; // Set to solvent/ion only if full of it.
		// ChemDB::SecondaryStructure::TYPE _secondaryStructure = ChemDB::SecondaryStructure::TYPE::COIL;
		// char							 _insertionCode		 = ' ';
	};
} // namespace VTX::Core::NewStruct
#endif
