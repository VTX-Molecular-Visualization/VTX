#ifndef __RESIDUE_HPP__
#define __RESIDUE_HPP__

#include "../defines.hpp"
#include <map>

namespace Model
{
	class ModelMolecule;
	class ModelChain;
	class ModelResidue
	{
	  public:
		enum RESIDUE_SYMBOL
		{
			UNKNOWN,
			ALA,
			ARG,
			ASN,
			ASP,
			CYS,
			GLN,
			GLU,
			GLY,
			HIS,
			ILE,
			LEU,
			LYS,
			MET,
			PHE,
			PRO,
			SER,
			THR,
			TRP,
			TYR,
			VAL,
			HOH,
			COUNT
		};

		// Static const mapping.
		static const std::string SYMBOL_NAME[ RESIDUE_SYMBOL::COUNT ];
		static const std::string SYMBOL_SHORT[ RESIDUE_SYMBOL::COUNT ];
		static const Vec3f *	 SYMBOL_COLOR[ RESIDUE_SYMBOL::COUNT ];

		inline ModelMolecule * const getMoleculePtr() const { return _moleculePtr; }
		inline void					 setMoleculePtr( ModelMolecule * const p_molecule ) { _moleculePtr = p_molecule; }
		inline ModelChain * const	 getChainPtr() const { return _chainPtr; }
		inline void					 setChainPtr( ModelChain * const p_chain ) { _chainPtr = p_chain; }

		inline const uint			getId() const { return _id; };
		inline void					setId( const uint p_id ) { _id = p_id; };
		inline const RESIDUE_SYMBOL getSymbol() const { return _symbol; };
		inline void					setSymbol( const RESIDUE_SYMBOL p_type ) { _symbol = p_type; };
		inline const std::string &	getSymbolName() const { return SYMBOL_NAME[ _symbol ]; }
		inline const std::string &	getSymbolShort() const { return SYMBOL_SHORT[ _symbol ]; }
		inline const Vec3f *		getColor() const { return SYMBOL_COLOR[ _symbol ]; }
		inline uint					getIdFirstAtom() const { return _idFirstAtom; };
		inline void					setIdFirstAtom( const uint p_id ) { _idFirstAtom = p_id; };
		inline uint					getAtomCount() const { return _atomCount; };
		inline void					setAtomCount( const uint p_count ) { _atomCount = p_count; };
		inline uint					getIdFirstBond() const { return _idFirstBond; };
		inline void					setIdFirstBond( const uint p_id ) { _idFirstBond = p_id; };
		inline uint					getBondCount() const { return _bondCount; };
		inline void					setBondCount( const uint p_count ) { _bondCount = p_count; };

	  private:
		ModelMolecule * _moleculePtr = nullptr;
		ModelChain *	_chainPtr	 = nullptr;

		uint		   _id			= INVALID_ID;
		RESIDUE_SYMBOL _symbol		= RESIDUE_SYMBOL::UNKNOWN;
		uint		   _idFirstAtom = 0;
		uint		   _atomCount	= 0;
		uint		   _idFirstBond = 0;
		uint		   _bondCount	= 0;
	};

} // namespace Model
#endif
