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
#include "id.hpp"
#include "model/secondary_structure.hpp"
#include <map>

namespace VTX
{
	namespace Model
	{
		class Molecule;
		class Chain;
		class Residue : public BaseModel, public Generic::BaseColorable, public Generic::BaseVisible, public Generic::BaseRepresentable
		{
			VTX_MODEL

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
				WAT,
				HOH,
				HEM,
				PO4,
				COUNT
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
			inline int				getIndexInOriginalChain() const { return _indexInOriginalChain; };
			inline void				setIndexInOriginalChain( const int p_index ) { _indexInOriginalChain = p_index; };
			inline Molecule * const getMoleculePtr() const { return _moleculePtr; }
			inline void				setMoleculePtr( Molecule * const p_molecule ) { _moleculePtr = p_molecule; }
			inline Chain * const	getChainPtr() const { return _chainPtr; }
			inline void				setChainPtr( Chain * const p_chain ) { _chainPtr = p_chain; }

			inline const SYMBOL		   getSymbol() const { return _symbol; };
			inline const std::string & getSymbolStr() const { return SYMBOL_STR[ (int)_symbol ]; }
			inline void				   setSymbol( const SYMBOL p_type )
			{
				_symbol = p_type;
				BaseModel::setDefaultName( &getSymbolName() );
			};
			inline const std::string & getSymbolName() const { return SYMBOL_NAME[ (int)_symbol ]; }
			inline const std::string & getSymbolShort() const { return SYMBOL_SHORT_STR[ (int)_symbol ]; }

			inline uint						 getIndexFirstAtom() const { return _indexFirstAtom; };
			inline void						 setIndexFirstAtom( const uint p_id ) { _indexFirstAtom = p_id; };
			inline uint						 getAtomCount() const { return _atomCount; };
			inline void						 setAtomCount( const uint p_count ) { _atomCount = p_count; };
			inline uint						 getIndexFirstBond() const { return _indexFirstBond; };
			inline void						 setIndexFirstBond( const uint p_id ) { _indexFirstBond = p_id; };
			inline uint						 getBondCount() const { return _bondCount; };
			inline void						 setBondCount( const uint p_count ) { _bondCount = p_count; };
			inline uint						 getIndiceFirstBond() const { return _indexFirstBond * 2; };
			inline uint						 getBondIndiceCount() const { return _bondCount * 2; };
			inline const std::vector<uint> & getIndexExtraBondStart() const { return _indexExtraBondStart; };
			inline std::vector<uint> &		 getIndexExtraBondStart() { return _indexExtraBondStart; };
			inline const std::vector<uint> & getIndexExtraBondEnd() const { return _indexExtraBondEnd; };
			inline std::vector<uint> &		 getIndexExtraBondEnd() { return _indexExtraBondEnd; };

			inline Atom::TYPE					   getAtomType() const { return _atomType; }
			inline void							   setAtomType( const Atom::TYPE p_atomType ) { _atomType = p_atomType; }
			inline const SecondaryStructure::VALUE getSecondaryStructure() const { return _secondaryStructure; };
			inline void							   setSecondaryStructure( const SecondaryStructure::VALUE p_structure ) { _secondaryStructure = p_structure; };
			const Atom * const					   findFirstAtomByName( const std::string & ) const;

			// Mask BaseVisible::setVisible
			void setVisible( const bool p_visible );

		  private:
			TYPE	   _type				 = TYPE::STANDARD;
			uint	   _index				 = 0;
			int		   _indexInOriginalChain = INT_MIN;
			Molecule * _moleculePtr			 = nullptr;
			Chain *	   _chainPtr			 = nullptr;

			SYMBOL _symbol = SYMBOL::UNKNOWN;

			uint					  _indexFirstAtom	   = 0;
			uint					  _atomCount		   = 0;
			uint					  _indexFirstBond	   = 0;
			uint					  _bondCount		   = 0;
			std::vector<uint>		  _indexExtraBondStart = std::vector<uint>();
			std::vector<uint>		  _indexExtraBondEnd   = std::vector<uint>();
			Atom::TYPE				  _atomType			   = Atom::TYPE::NORMAL; // Set to solvent/ion only if full of it.
			SecondaryStructure::VALUE _secondaryStructure  = SecondaryStructure::VALUE::COIL;

			Residue() : BaseModel( ID::Model::MODEL_RESIDUE ) {}
		};

	} // namespace Model
} // namespace VTX
#endif
