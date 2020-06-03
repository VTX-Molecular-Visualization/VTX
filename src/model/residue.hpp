#ifndef __VTX_MODEL_RESIDUE__
#define __VTX_MODEL_RESIDUE__

#ifdef _MSC_VER
#pragma once
#endif

#include "atom.hpp"
#include "base_model.hpp"
#include "define.hpp"
#include "generic/base_colorable.hpp"
#include "generic/base_representable.hpp"
#include "generic/base_visible.hpp"
#include <map>

namespace VTX
{
	namespace Model
	{
		class Molecule;
		class Chain;
		class Residue :
			public BaseModel,
			public Generic::BaseColorable,
			public Generic::BaseVisible,
			public Generic::BaseRepresentable
		{
		  public:
			enum class TYPE : int
			{
				STANDARD,
				NON_STANDARD
			};

			enum class SYMBOL : int
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
			static const std::string SYMBOL_STR[ (int)SYMBOL::COUNT ];
			static const std::string SYMBOL_SHORT_STR[ (int)SYMBOL::COUNT ];
			static const std::string SYMBOL_NAME[ (int)SYMBOL::COUNT ];
			static const Color::Rgb	 SYMBOL_COLOR[ (int)SYMBOL::COUNT ];

			inline TYPE				getType() const { return _type; }
			inline void				setType( const TYPE p_type ) { _type = p_type; }
			inline uint				getIndex() const { return _index; };
			inline void				setIndex( const uint p_index ) { _index = p_index; };
			inline Molecule * const getMoleculePtr() const { return _moleculePtr; }
			inline void				setMoleculePtr( Molecule * const p_molecule ) { _moleculePtr = p_molecule; }
			inline Chain * const	getChainPtr() const { return _chainPtr; }
			inline void				setChainPtr( Chain * const p_chain ) { _chainPtr = p_chain; }

			inline const SYMBOL		   getSymbol() const { return _symbol; };
			inline const std::string & getSymbolStr() const { return SYMBOL_STR[ (int)_symbol ]; }
			inline void				   setSymbol( const SYMBOL p_type ) { _symbol = p_type; };
			inline const std::string & getSymbolName() const { return SYMBOL_NAME[ (int)_symbol ]; }
			inline const std::string & getSymbolShort() const { return SYMBOL_SHORT_STR[ (int)_symbol ]; }

			inline uint						 getIdFirstAtom() const { return _idFirstAtom; };
			inline void						 setIdFirstAtom( const uint p_id ) { _idFirstAtom = p_id; };
			inline uint						 getAtomCount() const { return _atomCount; };
			inline void						 setAtomCount( const uint p_count ) { _atomCount = p_count; };
			inline uint						 getIdFirstBond() const { return _idFirstBond; };
			inline void						 setIdFirstBond( const uint p_id ) { _idFirstBond = p_id; };
			inline uint						 getBondCount() const { return _bondCount; };
			inline void						 setBondCount( const uint p_count ) { _bondCount = p_count; };
			inline Atom::TYPE				 getAtomType() const { return _atomType; }
			inline void						 setAtomType( const Atom::TYPE p_atomType ) { _atomType = p_atomType; }
			inline const SECONDARY_STRUCTURE getSecondaryStructure() const { return _secondaryStructure; };
			inline void						 setSecondaryStructure( const SECONDARY_STRUCTURE p_structure )
			{
				_secondaryStructure = p_structure;
			};
			inline const HANDEDNESS getHandedness() const { return _handedness; };
			inline void				setHandedness( const HANDEDNESS p_handedness ) { _handedness = p_handedness; };
			const Atom * const		findFirstAtomByName( const std::string & ) const;

			virtual void setSelected( const bool ) override;
			virtual void addRepresentation( const Generic::REPRESENTATION ) override;
			virtual void removeRepresentation( const Generic::REPRESENTATION ) override;

		  private:
			TYPE	   _type		= TYPE::STANDARD;
			uint	   _index		= 0;
			Molecule * _moleculePtr = nullptr;
			Chain *	   _chainPtr	= nullptr;

			SYMBOL _symbol = SYMBOL::UNKNOWN;

			uint				_idFirstAtom		= 0;
			uint				_atomCount			= 0;
			uint				_idFirstBond		= 0;
			uint				_bondCount			= 0;
			Atom::TYPE			_atomType			= Atom::TYPE::NORMAL; // Set to ion only if residue is full of ions.
			SECONDARY_STRUCTURE _secondaryStructure = SECONDARY_STRUCTURE::COIL;
			HANDEDNESS			_handedness			= HANDEDNESS::RIGHT;
		};

	} // namespace Model
} // namespace VTX
#endif
