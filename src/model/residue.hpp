#ifndef __VTX_RESIDUE__
#define __VTX_RESIDUE__

#include "../defines.hpp"
#include <map>

namespace VTX
{
	namespace Model
	{
		class Residue
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

			const uint			 getId() const { return _id; };
			void				 setId( const uint p_id ) { _id = p_id; };
			const RESIDUE_SYMBOL getSymbol() const { return _symbol; };
			void				 setSymbol( const RESIDUE_SYMBOL p_type ) { _symbol = p_type; };
			const std::string &	 getName() const { return SYMBOL_NAME[ _symbol ]; }
			const std::string &	 getSymbolShort() const { return SYMBOL_SHORT[ _symbol ]; }
			const Vec3f *		 getColor() const { return SYMBOL_COLOR[ _symbol ]; }

		  private:
			uint		   _id	   = INVALID_ID;
			RESIDUE_SYMBOL _symbol = RESIDUE_SYMBOL::UNKNOWN;
		};

	} // namespace Model
} // namespace VTX
#endif
