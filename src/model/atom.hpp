#ifndef __VTX_ATOM__
#define __VTX_ATOM__

#include "../defines.hpp"

namespace VTX
{
	namespace Model
	{
		class Atom
		{
		  public:
			// Sorted by atomic number.
			enum ATOM_SYMBOL
			{
				UNKNOWN = 0,
				H		= 1,
				HE		= 2,
				LI		= 3,
				BE		= 4,
				B		= 5,
				C		= 6,
				N		= 7,
				O		= 8,
				F		= 9,
				NE		= 10,
				NA		= 11,
				MG		= 12,
				AL		= 13,
				SI		= 14,
				P		= 15,
				S		= 16,
				CL		= 17,
				AR		= 18,
				K		= 19,
				CA		= 20,
				SC		= 21,
				TI		= 22,
				V		= 23,
				CR		= 24,
				MN		= 25,
				FE		= 26,
				CO		= 27,
				NI		= 28,
				CU		= 29,
				ZN		= 30,
				GA		= 31,
				GE		= 32,
				AS		= 33,
				SE		= 34,
				BR		= 35,
				KR		= 36,
				RB		= 37,
				SR		= 38,
				Y		= 39,
				ZR		= 40,
				NB		= 41,
				MO		= 42,
				TC		= 43,
				RU		= 44,
				RH		= 45,
				PD		= 46,
				AG		= 47,
				CD		= 48,
				IN		= 49,
				SN		= 50,
				SB		= 51,
				TE		= 52,
				I		= 53,
				XE		= 54,
				CS		= 55,
				BA		= 56,
				LA		= 57,
				CE		= 58,
				PR		= 59,
				ND		= 60,
				PM		= 61,
				SM		= 62,
				EU		= 63,
				GD		= 64,
				TB		= 65,
				DY		= 66,
				HO		= 67,
				ER		= 68,
				TM		= 69,
				YB		= 70,
				LU		= 71,
				HF		= 72,
				TA		= 73,
				W		= 74,
				RE		= 75,
				OS		= 76,
				IR		= 77,
				PT		= 78,
				AU		= 79,
				HG		= 80,
				TL		= 81,
				PB		= 82,
				BI		= 83,
				PO		= 84,
				AT		= 85,
				RN		= 86,
				FR		= 87,
				RA		= 88,
				AC		= 89,
				TH		= 90,
				PA		= 91,
				U		= 92,
				NP		= 93,
				PU		= 94,
				AM		= 95,
				CM		= 96,
				BK		= 97,
				CF		= 98,
				ES		= 99,
				FM		= 100,
				MD		= 101,
				NO		= 102,
				LR		= 103,
				RF		= 104,
				DD		= 105,
				SG		= 106,
				BHJ		= 107,
				HS		= 108,
				MT		= 109,
				DS		= 110,
				RG		= 111,
				UUB		= 112,
				UUT		= 113,
				UUQ		= 114,
				UUP		= 115,
				UUH		= 116,
				UUS		= 117,
				UUO		= 118,
				COUNT
			};

			// Static const mapping.
			static const std::string SYMBOL_NAME[ ATOM_SYMBOL::COUNT ];
			static const float		 SYMBOL_VDW_RADIUS[ ATOM_SYMBOL::COUNT ];
			static const Vec3f *	 SYMBOL_COLOR[ ATOM_SYMBOL::COUNT ];

			const uint			getId() const { return _id; };
			void				setId( const uint p_id ) { _id = p_id; };
			const ATOM_SYMBOL	getSymbol() const { return _symbol; };
			void				setSymbol( const ATOM_SYMBOL p_symbol ) { _symbol = p_symbol; };
			const std::string & getName() const { return SYMBOL_NAME[ _symbol ]; }
			const uint			getAtomicNumber() const { return (uint)_symbol; }
			const float			getVdwRadius() const { return SYMBOL_VDW_RADIUS[ _symbol ]; }
			const Vec3f *		getColor() const { return SYMBOL_COLOR[ _symbol ]; }

		  private:
			uint		_id		= INVALID_ID;
			ATOM_SYMBOL _symbol = ATOM_SYMBOL::UNKNOWN;
		};

	} // namespace Model
} // namespace VTX
#endif
