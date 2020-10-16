#ifndef __VTX_MODEL_ATOM__
#define __VTX_MODEL_ATOM__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_model.hpp"
#include "define.hpp"
#include "generic/base_colorable.hpp"
#include "id.hpp"

namespace VTX
{
	namespace Model
	{
		class Molecule;
		class Chain;
		class Residue;
		class Atom : public BaseModel, public Generic::BaseColorable
		{
		  public:
			// Sorted by atomic number.
			enum class SYMBOL : int
			{
				UNKNOWN = 0,
				A_H		= 1,
				A_HE	= 2,
				A_LI	= 3,
				A_BE	= 4,
				A_B		= 5,
				A_C		= 6,
				A_N		= 7,
				A_O		= 8,
				A_F		= 9,
				A_NE	= 10,
				A_NA	= 11,
				A_MG	= 12,
				A_AL	= 13,
				A_SI	= 14,
				A_P		= 15,
				A_S		= 16,
				A_CL	= 17,
				A_AR	= 18,
				A_K		= 19,
				A_CA	= 20,
				A_SC	= 21,
				A_TI	= 22,
				A_V		= 23,
				A_CR	= 24,
				A_MN	= 25,
				A_FE	= 26,
				A_CO	= 27,
				A_NI	= 28,
				A_CU	= 29,
				A_ZN	= 30,
				A_GA	= 31,
				A_GE	= 32,
				A_AS	= 33,
				A_SE	= 34,
				A_BR	= 35,
				A_KR	= 36,
				A_RB	= 37,
				A_SR	= 38,
				A_Y		= 39,
				A_ZR	= 40,
				A_NB	= 41,
				A_MO	= 42,
				A_TC	= 43,
				A_RU	= 44,
				A_RH	= 45,
				A_PD	= 46,
				A_AG	= 47,
				A_CD	= 48,
				A_IN	= 49,
				A_SN	= 50,
				A_SB	= 51,
				A_TE	= 52,
				A_I		= 53,
				A_XE	= 54,
				A_CS	= 55,
				A_BA	= 56,
				A_LA	= 57,
				A_CE	= 58,
				A_PR	= 59,
				A_ND	= 60,
				A_PM	= 61,
				A_SM	= 62,
				A_EU	= 63,
				A_GD	= 64,
				A_TB	= 65,
				A_DY	= 66,
				A_HO	= 67,
				A_ER	= 68,
				A_TM	= 69,
				A_YB	= 70,
				A_LU	= 71,
				A_HF	= 72,
				A_TA	= 73,
				A_W		= 74,
				A_RE	= 75,
				A_OS	= 76,
				A_IR	= 77,
				A_PT	= 78,
				A_AU	= 79,
				A_HG	= 80,
				A_TL	= 81,
				A_PB	= 82,
				A_BI	= 83,
				A_PO	= 84,
				A_AT	= 85,
				A_RN	= 86,
				A_FR	= 87,
				A_RA	= 88,
				A_AC	= 89,
				A_TH	= 90,
				A_PA	= 91,
				A_U		= 92,
				A_NP	= 93,
				A_PU	= 94,
				A_AM	= 95,
				A_CM	= 96,
				A_BK	= 97,
				A_CF	= 98,
				A_ES	= 99,
				A_FM	= 100,
				A_MD	= 101,
				A_NO	= 102,
				A_LR	= 103,
				A_RF	= 104,
				A_DD	= 105,
				A_SG	= 106,
				A_BHJ	= 107,
				A_HS	= 108,
				A_MT	= 109,
				A_DS	= 110,
				A_RG	= 111,
				A_UUB	= 112,
				A_UUT	= 113,
				A_UUQ	= 114,
				A_UUP	= 115,
				A_UUH	= 116,
				A_UUS	= 117,
				A_UUO	= 118,
				COUNT
			};

			enum class TYPE : int
			{
				NORMAL,
				SOLVENT,
				ION
			};

			// Static const mapping.
			static const std::string SYMBOL_STR[ (int)SYMBOL::COUNT ];
			static const std::string SYMBOL_NAME[ (int)SYMBOL::COUNT ];
			static const float		 SYMBOL_VDW_RADIUS[ (int)SYMBOL::COUNT ];
			static const Color::Rgb	 SYMBOL_COLOR[ (int)SYMBOL::COUNT ];

			Atom() : BaseModel( ID::Model::MODEL_ATOM ) {};

			inline const uint		getIndex() const { return _index; };
			inline void				setIndex( const uint p_index ) { _index = p_index; };
			inline Molecule * const getMoleculePtr() const { return _moleculePtr; }
			inline void				setMoleculePtr( Molecule * const p_molecule ) { _moleculePtr = p_molecule; }
			inline Chain * const	getChainPtr() const { return _chainPtr; }
			inline void				setChainPtr( Chain * const p_chain ) { _chainPtr = p_chain; }
			inline Residue * const	getResiduePtr() const { return _residuePtr; }
			inline void				setResiduePtr( Residue * const p_residue ) { _residuePtr = p_residue; }

			inline const SYMBOL		   getSymbol() const { return _symbol; };
			inline const std::string & getSymbolStr() const { return SYMBOL_STR[ (int)_symbol ]; };
			inline void				   setSymbol( const SYMBOL p_symbol )
			{
				_symbol = p_symbol;
				BaseModel::setDefaultName( &getSymbolName() );
			};
			inline const std::string & getSymbolName() const { return SYMBOL_NAME[ (int)_symbol ]; }
			inline const uint		   getAtomicNumber() const { return (uint)_symbol; }
			inline const float		   getVdwRadius() const { return SYMBOL_VDW_RADIUS[ (int)_symbol ]; }
			inline TYPE				   getType() const { return _type; }
			inline void				   setType( const TYPE p_type ) { _type = p_type; }
			inline const std::string & getName() const { return _name; };
			inline void				   setName( const std::string & p_name ) { _name = p_name; };

			void setSelected( const bool );

		  private:
			uint	   _index		= 0;
			Molecule * _moleculePtr = nullptr;
			Chain *	   _chainPtr	= nullptr;
			Residue *  _residuePtr	= nullptr;
			TYPE	   _type		= TYPE::NORMAL;
			SYMBOL	   _symbol		= SYMBOL::UNKNOWN;
			// /!\ Names PDB != MMTF (CA and C1 for alpha carbon).
			std::string _name = "";
		};

	} // namespace Model
} // namespace VTX
#endif
