#ifndef __VTX_CORE_STRUCT_RESIDUE__
#define __VTX_CORE_STRUCT_RESIDUE__

#include "_fwd.hpp"
#include "core/chemdb/atom.hpp"
#include "core/chemdb/residue.hpp"
#include "core/chemdb/secondary_structure.hpp"
#include <string>
#include <util/constants.hpp>
#include <util/types.hpp>

namespace VTX::Core::Struct
{
	class Residue
	{
	  public:
		static const int SYMBOL_COUNT = int( ChemDB::Residue::SYMBOL::COUNT );

		static bool					   checkIfStandardFromName( const std::string & p_residueName );
		static const Util::Color::Rgba getResidueColor( const Residue & p_residue );

		Residue() {}

		inline bool isStandardResidue() const
		{
			return int( ChemDB::Residue::SYMBOL::ALA ) <= _symbol && _symbol <= int( ChemDB::Residue::SYMBOL::PYL );
		}
		inline bool isWater() const
		{
			return ( _symbol == int( ChemDB::Residue::SYMBOL::WAT ) )
				   || ( _symbol == int( ChemDB::Residue::SYMBOL::HOH ) );
		}

		inline ChemDB::Residue::TYPE getType() const { return _type; }
		inline void					 setType( const ChemDB::Residue::TYPE p_type ) { _type = p_type; }
		inline uint					 getIndex() const { return _index; };
		inline void					 setIndex( const uint p_index ) { _index = p_index; };
		inline int					 getIndexInOriginalChain() const { return _indexInOriginalChain; };
		inline void					 setIndexInOriginalChain( const int p_index ) { _indexInOriginalChain = p_index; };
		Molecule * const			 getMoleculePtr() const;
		inline Chain * const		 getChainPtr() const { return _chainPtr; }
		void						 setChainPtr( Chain * const p_chain );

		inline const ChemDB::Residue::SYMBOL getSymbol() const { return ChemDB::Residue::SYMBOL( _symbol ); };
		const std::string &					 getSymbolStr() const;
		void								 setSymbol( const ChemDB::Residue::SYMBOL & p_symbol );
		void								 setSymbol( const int p_symbolValue );
		const std::string &					 getSymbolName() const;
		const std::string &					 getSymbolShort() const;

		inline uint				  getIndexFirstAtom() const { return _indexFirstAtom; };
		inline void				  setIndexFirstAtom( const uint p_id ) { _indexFirstAtom = p_id; };
		inline uint				  getAtomCount() const { return _atomCount; };
		void					  setAtomCount( const uint p_count );
		inline uint				  getIndexFirstBond() const { return _indexFirstBond; };
		inline void				  setIndexFirstBond( const uint p_id ) { _indexFirstBond = p_id; };
		inline uint				  getBondCount() const { return _bondCount; };
		inline void				  setBondCount( const uint p_count ) { _bondCount = p_count; };
		inline uint				  getRealAtomCount() const { return _realAtomCount; };
		void					  removeToAtom( const uint p_atomIndex );
		inline char				  getInsertionCode() const { return _insertionCode; }
		inline void				  setInsertionCode( char p_insertionCode ) { _insertionCode = p_insertionCode; }
		inline bool				  hasInsertionCode() const { return _insertionCode != ' '; }
		inline ChemDB::Atom::TYPE getAtomType() const { return _atomType; }
		inline void				  setAtomType( const ChemDB::Atom::TYPE p_atomType ) { _atomType = p_atomType; }
		inline const ChemDB::SecondaryStructure::TYPE getSecondaryStructure() const { return _secondaryStructure; };
		inline void setSecondaryStructure( const ChemDB::SecondaryStructure::TYPE p_structure )
		{
			_secondaryStructure = p_structure;
		};
		const Atom * const findFirstAtomByName( const std::string & ) const;
		const uint		   findBondIndex( const uint p_firstAtomIndex, const uint p_secondAtomIndex ) const;

		const Atom * const getAlphaCarbon() const;

	  private:
		ChemDB::Residue::TYPE _type					= ChemDB::Residue::TYPE::STANDARD;
		uint				  _index				= INVALID_ID;
		int					  _indexInOriginalChain = 0;
		Chain *				  _chainPtr				= nullptr;

		int _symbol = int( ChemDB::Residue::SYMBOL::UNKNOWN );

		uint			   _indexFirstAtom = INVALID_ID;
		uint			   _atomCount	   = 0;
		uint			   _realAtomCount  = 0;
		uint			   _indexFirstBond = INVALID_ID;
		uint			   _bondCount	   = 0;
		ChemDB::Atom::TYPE _atomType	   = ChemDB::Atom::TYPE::NORMAL; // Set to solvent/ion only if full of it.
		ChemDB::SecondaryStructure::TYPE _secondaryStructure = ChemDB::SecondaryStructure::TYPE::COIL;
		char							 _insertionCode		 = ' ';
	};
} // namespace VTX::Core::Struct
#endif
