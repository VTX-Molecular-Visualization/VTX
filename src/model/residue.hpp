#ifndef __VTX_MODEL_RESIDUE__
#define __VTX_MODEL_RESIDUE__

#ifdef _MSC_VER
#pragma once
#endif

#include "atom.hpp"
#include "base_model.hpp"
#include "define.hpp"
#include "generic/base_colorable.hpp"
#include "generic/base_visible.hpp"
#include <map>

namespace VTX
{
	namespace Model
	{
		class Molecule;
		class Chain;
		class Residue : public BaseModel, public Generic::BaseColorable, public Generic::BaseVisible
		{
		  public:
			enum class RESIDUE_SYMBOL : int
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
				ASX,
				GLX,
				COUNT
			};

			enum class RESIDUE_TYPE : int
			{
				NORMAL,
				LIGAND
			};

			enum class SECONDARY_STRUCTURE : int
			{
				COIL,
				HELIX,
				STRAND
			};

			enum class HANDEDNESS : int
			{
				LEFT  = -1,
				RIGHT = 1
			};

			// Static const mapping.
			static const std::string SYMBOL_STR[ (int)RESIDUE_SYMBOL::COUNT ];
			static const std::string SYMBOL_SHORT_STR[ (int)RESIDUE_SYMBOL::COUNT ];
			static const std::string SYMBOL_NAME[ (int)RESIDUE_SYMBOL::COUNT ];
			static const Color::Rgb	 SYMBOL_COLOR[ (int)RESIDUE_SYMBOL::COUNT ];

			inline uint				getIndex() const { return _index; };
			inline void				setIndex( const uint p_index ) { _index = p_index; };
			inline Molecule * const getMoleculePtr() const { return _moleculePtr; }
			inline void				setMoleculePtr( Molecule * const p_molecule ) { _moleculePtr = p_molecule; }
			inline Chain * const	getChainPtr() const { return _chainPtr; }
			inline void				setChainPtr( Chain * const p_chain ) { _chainPtr = p_chain; }

			inline const RESIDUE_SYMBOL		 getSymbol() const { return _symbol; };
			inline const std::string &		 getSymbolStr() const { return SYMBOL_STR[ (int)_symbol ]; }
			inline void						 setSymbol( const RESIDUE_SYMBOL p_type ) { _symbol = p_type; };
			inline const std::string &		 getSymbolName() const { return SYMBOL_NAME[ (int)_symbol ]; }
			inline const std::string &		 getSymbolShort() const { return SYMBOL_SHORT_STR[ (int)_symbol ]; }
			inline RESIDUE_TYPE				 getType() const { return _type; }
			inline void						 setType( const RESIDUE_TYPE p_type ) { _type = p_type; }
			inline uint						 getIdFirstAtom() const { return _idFirstAtom; };
			inline void						 setIdFirstAtom( const uint p_id ) { _idFirstAtom = p_id; };
			inline uint						 getAtomCount() const { return _atomCount; };
			inline void						 setAtomCount( const uint p_count ) { _atomCount = p_count; };
			inline const SECONDARY_STRUCTURE getSecondaryStructure() const { return _secondaryStructure; };
			inline void						 setSecondaryStructure( const SECONDARY_STRUCTURE p_structure )
			{
				_secondaryStructure = p_structure;
			};
			inline const HANDEDNESS getHandedness() const { return _handedness; };
			inline void				setHandedness( const HANDEDNESS p_handedness ) { _handedness = p_handedness; };
			const Atom * const		findFirstAtomByName( const std::string & ) const;

			virtual void setSelected( const bool ) override;

		  private:
			uint	   _index		= 0;
			Molecule * _moleculePtr = nullptr;
			Chain *	   _chainPtr	= nullptr;

			RESIDUE_SYMBOL		_symbol				= RESIDUE_SYMBOL::UNKNOWN;
			RESIDUE_TYPE		_type				= RESIDUE_TYPE::NORMAL;
			uint				_idFirstAtom		= 0;
			uint				_atomCount			= 0;
			SECONDARY_STRUCTURE _secondaryStructure = SECONDARY_STRUCTURE::COIL;
			HANDEDNESS			_handedness			= HANDEDNESS::RIGHT;
			// uint		   _idFirstBond = 0;
			// uint		   _bondCount	= 0;
		};

	} // namespace Model
} // namespace VTX
#endif
