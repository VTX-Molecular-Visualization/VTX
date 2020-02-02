#ifndef __VTX_RESIDUE__
#define __VTX_RESIDUE__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "base_model.hpp"
#include <map>

namespace VTX
{
	namespace Model
	{
		class ModelMolecule;
		class ModelChain;
		class ModelResidue : public BaseModel
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
				HOH,
				COUNT
			};

			// Static const mapping.
			static const std::string SYMBOL_NAME[ (int)RESIDUE_SYMBOL::COUNT ];
			static const std::string SYMBOL_SHORT[ (int)RESIDUE_SYMBOL::COUNT ];
			static const Vec3f *	 SYMBOL_COLOR[ (int)RESIDUE_SYMBOL::COUNT ];

			inline ModelMolecule * const getMoleculePtr() const { return _moleculePtr; }
			inline void				  setMoleculePtr( ModelMolecule * const p_molecule ) { _moleculePtr = p_molecule; }
			inline ModelChain * const getChainPtr() const { return _chainPtr; }
			inline void				  setChainPtr( ModelChain * const p_chain ) { _chainPtr = p_chain; }

			inline const RESIDUE_SYMBOL getSymbol() const { return _symbol; };
			inline void					setSymbol( const RESIDUE_SYMBOL p_type ) { _symbol = p_type; };
			inline const std::string &	getSymbolName() const { return SYMBOL_NAME[ (int)_symbol ]; }
			inline const std::string &	getSymbolShort() const { return SYMBOL_SHORT[ (int)_symbol ]; }
			inline float * const		getColor() { return _color; }
			inline void					setColor( const Vec3f & p_color )
			{
				_color[ 0 ] = p_color.x;
				_color[ 1 ] = p_color.y;
				_color[ 2 ] = p_color.z;
			}
			inline uint getIdFirstAtom() const { return _idFirstAtom; };
			inline void setIdFirstAtom( const uint p_id ) { _idFirstAtom = p_id; };
			inline uint getAtomCount() const { return _atomCount; };
			inline void setAtomCount( const uint p_count ) { _atomCount = p_count; };
			inline uint getIdFirstBond() const { return _idFirstBond; };
			inline void setIdFirstBond( const uint p_id ) { _idFirstBond = p_id; };
			inline uint getBondCount() const { return _bondCount; };
			inline void setBondCount( const uint p_count ) { _bondCount = p_count; };

			virtual void setSelected( const bool ) override;

		  private:
			ModelMolecule * _moleculePtr = nullptr;
			ModelChain *	_chainPtr	 = nullptr;

			RESIDUE_SYMBOL _symbol		= RESIDUE_SYMBOL::UNKNOWN;
			float		   _color[ 3 ]	= { 0.f, 0.f, 0.f };
			uint		   _idFirstAtom = 0;
			uint		   _atomCount	= 0;
			uint		   _idFirstBond = 0;
			uint		   _bondCount	= 0;
		};

	} // namespace Model
} // namespace VTX
#endif
