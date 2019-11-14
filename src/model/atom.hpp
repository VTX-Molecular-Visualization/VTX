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
				He		= 2,
				Li		= 3,
				Be		= 4,
				B		= 5,
				C		= 6,
				N		= 7,
				O		= 8,
				F		= 9,
				Ne		= 10,
				Na		= 11,
				Mg		= 12,
				Al		= 13,
				Si		= 14,
				P		= 15,
				S		= 16,
				Cl		= 17,
				Ar		= 18,
				K		= 19,
				Ca		= 20,
				Sc		= 21,
				Ti		= 22,
				V		= 23,
				Cr		= 24,
				Mn		= 25,
				Fe		= 26,
				Co		= 27,
				Ni		= 28,
				Cu		= 29,
				Zn		= 30,
				Ga		= 31,
				Ge		= 32,
				As		= 33,
				Se		= 34,
				Br		= 35,
				Kr		= 36,
				Rb		= 37,
				Sr		= 38,
				Y		= 39,
				Zr		= 40,
				Nb		= 41,
				Mo		= 42,
				Tc		= 43,
				Ru		= 44,
				Rh		= 45,
				Pd		= 46,
				Ag		= 47,
				Cd		= 48,
				In		= 49,
				Sn		= 50,
				Sb		= 51,
				Te		= 52,
				I		= 53,
				Xe		= 54,
				Cs		= 55,
				Ba		= 56,
				La		= 57,
				Ce		= 58,
				Pr		= 59,
				Nd		= 60,
				Pm		= 61,
				Sm		= 62,
				Eu		= 63,
				Gd		= 64,
				Tb		= 65,
				Dy		= 66,
				Ho		= 67,
				Er		= 68,
				Tm		= 69,
				Yb		= 70,
				Lu		= 71,
				Hf		= 72,
				Ta		= 73,
				W		= 74,
				Re		= 75,
				Os		= 76,
				Ir		= 77,
				Pt		= 78,
				Au		= 79,
				Hg		= 80,
				Tl		= 81,
				Pb		= 82,
				Bi		= 83,
				Po		= 84,
				At		= 85,
				Rn		= 86,
				Fr		= 87,
				Ra		= 88,
				Ac		= 89,
				Th		= 90,
				Pa		= 91,
				U		= 92,
				Np		= 93,
				Pu		= 94,
				Am		= 95,
				Cm		= 96,
				Bk		= 97,
				Cf		= 98,
				Es		= 99,
				Fm		= 100,
				Md		= 101,
				No		= 102,
				Lr		= 103,
				Rf		= 104,
				Dd		= 105,
				Sg		= 106,
				Bhj		= 107,
				Hs		= 108,
				Mt		= 109,
				Ds		= 110,
				Rg		= 111,
				Uub		= 112,
				Uut		= 113,
				Uuq		= 114,
				Uup		= 115,
				Uuh		= 116,
				Uus		= 117,
				Uuo		= 118,
				COUNT
			};

			// Static const mapping.
			static const std::string SYMBOL_NAME[ ATOM_SYMBOL::COUNT ];
			static const float		 SYMBOL_VDW_RADII[ ATOM_SYMBOL::COUNT ];
			static const Vec3f		 SYMBOL_COLOR[ ATOM_SYMBOL::COUNT ];

			uint		getId() const { return _id; };
			void		setId( const uint p_id ) { _id = p_id; };
			ATOM_SYMBOL getSymbol() const { return _symbol; };
			void		setSymbol( const ATOM_SYMBOL p_symbol )
			{
				_symbol = p_symbol;
			};
			const Vec3f & getPosition() const { return _position; };
			void		  setPosition( const Vec3f & p_position )
			{
				_position = p_position;
			};

		  private:
			uint		_id		  = INVALID_ID;
			ATOM_SYMBOL _symbol	  = ATOM_SYMBOL::UNKNOWN;
			Vec3f		_position = VEC3F_ZERO;
		};

	} // namespace Model
} // namespace VTX
#endif
