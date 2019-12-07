#include "model_atom.hpp"
#include "../util/type.hpp"

namespace VTX
{
	namespace Model
	{
		void ModelAtom::setSelected( const bool p_selected )
		{
			_isSelected = p_selected;
			if ( isSelected() ) { _addView( Util::Type::componentToView<ModelAtom>( UI::COMPONENT_NAME::VIEW_ATOM ) ); }
			else
			{
				_clearViews();
			}
		}

		const std::string ModelAtom::SYMBOL_NAME[ (int)ATOM_SYMBOL::COUNT ] = {
			"Unknown",		 // UNKNOWN = 0,
			"Hydrogen",		 // H		= 1,
			"Helium",		 // HE		= 2,
			"Lithium",		 // LI		= 3,
			"Beryllium",	 // BE		= 4,
			"Boron",		 // B		= 5,
			"Carbon",		 // C		= 6,
			"Nitrogen",		 // N		= 7,
			"Oxygen",		 // O		= 8,
			"Fluorine",		 // F		= 9,
			"Neon",			 // NE		= 10,
			"Sodium",		 // NA		= 11,
			"Magnesium",	 // MG		= 12,
			"Aluminum",		 // AL		= 13,
			"Silicon",		 // SI		= 14,
			"Phosphorus",	 // P		= 15,
			"Sulfur",		 // S		= 16,
			"Chlorine",		 // CL		= 17,
			"Argon",		 // AR		= 18,
			"Potassium",	 // K		= 19,
			"Calcium",		 // CA		= 20,
			"Scandium",		 // SC		= 21,
			"Titanium",		 // TI		= 22,
			"Vanadium",		 // V		= 23,
			"Chromium",		 // CR		= 24,
			"Manganese",	 // MN		= 25,
			"Iron",			 // FE		= 26,
			"Cobalt",		 // CO		= 27,
			"Nickel",		 // NI		= 28,
			"Copper",		 // CU		= 29,
			"Zinc",			 // ZN		= 30,
			"Gallium",		 // GA		= 31,
			"Germanium",	 // GE		= 32,
			"Arsenic",		 // AS		= 33,
			"Selenium",		 // SE		= 34,
			"Bromine",		 // BR		= 35,
			"Krypton",		 // KR		= 36,
			"Rubidium",		 // RB		= 37,
			"Strontium",	 // SR		= 38,
			"Yttrium",		 // Y		= 39,
			"Zirconium",	 // ZR		= 40,
			"Niobium",		 // NB		= 41,
			"Molybdenum",	 // MO		= 42,
			"Technetium",	 // TC		= 43,
			"Ruthenium",	 // RU		= 44,
			"Rhodium",		 // RH		= 45,
			"Palladium",	 // PD		= 46,
			"Silver",		 // AG		= 47,
			"Cadmium",		 // CD		= 48,
			"Indium",		 // IN		= 49,
			"Tin",			 // SN		= 50,
			"Antimony",		 // SB		= 51,
			"Tellurium",	 // TE		= 52,
			"Iodine",		 // I		= 53,
			"Xenon",		 // XE		= 54,
			"Cesium",		 // CS		= 55,
			"Barium",		 // BA		= 56,
			"Lanthanum",	 // LA		= 57,
			"Cerium",		 // CE		= 58,
			"Praseodymium",	 // PR		= 59,
			"Neodymium",	 // ND		= 60,
			"Promethium",	 // PM		= 61,
			"Samarium",		 // SM		= 62,
			"Europium",		 // EU		= 63,
			"Gadolinium",	 // GD		= 64,
			"Terbium",		 // TB		= 65,
			"Dysprosium",	 // DY		= 66,
			"Holmium",		 // HO		= 67,
			"Erbium",		 // ER		= 68,
			"Thulium",		 // TM		= 69,
			"Ytterbium",	 // YB		= 70,
			"Lutetium",		 // LU		= 71,
			"Hafnium",		 // HF		= 72,
			"Tantalum",		 // TA		= 73,
			"Tungsten",		 // W		= 74,
			"Rhenium",		 // RE		= 75,
			"Osmium",		 // OS		= 76,
			"Iridium",		 // IR		= 77,
			"Platinum",		 // PT		= 78,
			"Gold",			 // AU		= 79,
			"Mercury",		 // HG		= 80,
			"Thallium",		 // TL		= 81,
			"Lead",			 // PB		= 82,
			"Bismuth",		 // BI		= 83,
			"Polonium",		 // PO		= 84,
			"Astatine",		 // AT		= 85,
			"Radon",		 // RN		= 86,
			"Francium",		 // FR		= 87,
			"Radium",		 // RA		= 88,
			"Actinium",		 // AC		= 89,
			"Thorium",		 // TH		= 90,
			"Protactinium",	 // PA		= 91,
			"Uranium",		 // U		= 92,
			"Neptunium",	 // NP		= 93,
			"Plutonium",	 // PU		= 94,
			"Americium",	 // AM		= 95,
			"Curium",		 // CM		= 96,
			"Berkelium",	 // BK		= 97,
			"Californium",	 // CF		= 98,
			"Einsteinium",	 // ES		= 99,
			"Fermium",		 // FM		= 100,
			"Mendelevium",	 // MD		= 101,
			"Nobelium",		 // NO		= 102,
			"Lawrencium",	 // LR		= 103,
			"Rutherfordium", // RF		= 104,
			"Dubnium",		 // DD		= 105,
			"Seaborgium",	 // SG		= 106,
			"Bohrium",		 // BHJ		= 107,
			"Hassium",		 // HS		= 108,
			"Meitnerium",	 // MT		= 109,
			"Darmstadtium",	 // DS		= 110,
			"Roentgenium",	 // RG		= 111,
			"Ununbium",		 // UUB		= 112,
			"Ununtrium",	 // UUT		= 113,
			"Ununquadium",	 // UUQ		= 114,
			"Ununpentium",	 // UUP		= 115,
			"Ununhexium",	 // UUH		= 116,
			"Ununseptium",	 // UUS		= 117,
			"Ununoctium"	 // UUO		= 118,
		};

		const float ModelAtom::SYMBOL_VDW_RADIUS[ (int)ATOM_SYMBOL::COUNT ] = {
			0.00f, // UNKNOWN	= 0,
			1.20f, // H			= 1,
			1.43f, // HE		= 2,
			2.12f, // LI		= 3,
			1.98f, // BE		= 4,
			1.91f, // B			= 5,
			1.77f, // C			= 6,
			1.66f, // N			= 7,
			1.50f, // O			= 8,
			1.46f, // F			= 9,
			1.58f, // NE		= 10,
			2.50f, // NA		= 11,
			2.51f, // MG		= 12,
			2.25f, // AL		= 13,
			2.19f, // SI		= 14,
			1.90f, // P			= 15,
			1.89f, // S			= 16,
			1.82f, // CL		= 17,
			1.83f, // AR		= 18,
			2.73f, // K			= 19,
			2.62f, // CA		= 20,
			2.58f, // SC		= 21,
			2.46f, // TI		= 22,
			2.42f, // V			= 23,
			2.45f, // CR		= 24,
			2.45f, // MN		= 25,
			2.44f, // FE		= 26,
			2.40f, // CO		= 27,
			2.40f, // NI		= 28,
			2.38f, // CU		= 29,
			2.39f, // ZN		= 30,
			2.32f, // GA		= 31,
			2.29f, // GE		= 32,
			1.88f, // AS		= 33,
			1.82f, // SE		= 34,
			1.86f, // BR		= 35,
			2.25f, // KR		= 36,
			3.21f, // RB		= 37,
			2.84f, // SR		= 38,
			2.75f, // Y			= 39,
			2.52f, // ZR		= 40,
			2.56f, // NB		= 41,
			2.45f, // MO		= 42,
			2.44f, // TC		= 43,
			2.46f, // RU		= 44,
			2.44f, // RH		= 45,
			2.15f, // PD		= 46,
			2.53f, // AG		= 47,
			2.49f, // CD		= 48,
			2.43f, // IN		= 49,
			2.42f, // SN		= 50,
			2.47f, // SB		= 51,
			1.99f, // TE		= 52,
			2.04f, // I			= 53,
			2.06f, // XE		= 54,
			3.48f, // CS		= 55,
			3.03f, // BA		= 56,
			2.98f, // LA		= 57,
			2.88f, // CE		= 58,
			2.92f, // PR		= 59,
			2.95f, // ND		= 60,
			0.00f, // PM		= 61,
			2.90f, // SM		= 62,
			2.87f, // EU		= 63,
			2.83f, // GD		= 64,
			2.79f, // TB		= 65,
			2.87f, // DY		= 66,
			2.81f, // HO		= 67,
			2.83f, // ER		= 68,
			2.79f, // TM		= 69,
			2.80f, // YB		= 70,
			2.74f, // LU		= 71,
			2.63f, // HF		= 72,
			2.53f, // TA		= 73,
			2.57f, // W			= 74,
			2.49f, // RE		= 75,
			2.48f, // OS		= 76,
			2.41f, // IR		= 77,
			2.29f, // PT		= 78,
			2.32f, // AU		= 79,
			2.45f, // HG		= 80,
			2.47f, // TL		= 81,
			2.60f, // PB		= 82,
			2.54f, // BI		= 83,
			0.00f, // PO		= 84,
			0.00f, // AT		= 85,
			0.00f, // RN		= 86,
			0.00f, // FR		= 87,
			0.00f, // RA		= 88,
			2.80f, // AC		= 89,
			2.93f, // TH		= 90,
			2.88f, // PA		= 91,
			2.71f, // U			= 92,
			2.82f, // NP		= 93,
			2.81f, // PU		= 94,
			2.83f, // AM		= 95,
			3.05f, // CM		= 96,
			3.40f, // BK		= 97,
			3.05f, // CF		= 98,
			2.70f, // ES		= 99,
			0.00f, // FM		= 100,
			0.00f, // MD		= 101,
			0.00f, // NO		= 102,
			0.00f, // LR		= 103,
			0.00f, // RF		= 104,
			0.00f, // DD		= 105,
			0.00f, // SG		= 106,
			0.00f, // BHJ		= 107,
			0.00f, // HS		= 108,
			0.00f, // MT		= 109,
			0.00f, // DS		= 110,
			0.00f, // RG		= 111,
			0.00f, // UUB		= 112,
			0.00f, // UUT		= 113,
			0.00f, // UUQ		= 114,
			0.00f, // UUP		= 115,
			0.00f, // UUH		= 116,
			0.00f, // UUS		= 117,
			0.00f  // UUO		= 118,
		};
		const Vec3f * ModelAtom::SYMBOL_COLOR[ (int)ATOM_SYMBOL::COUNT ] = {
			&COLOR_BLACK,	   // UNKNOWN	= 0,
			&COLOR_WHITE,	   // H			= 1,
			&COLOR_MAGENTA,	   // HE		= 2,
			&COLOR_MAGENTA,	   // LI		= 3,
			&COLOR_MAGENTA,	   // BE		= 4,
			&COLOR_MAGENTA,	   // B			= 5,
			&COLOR_GREY,	   // C			= 6,
			&COLOR_BLUE,	   // N			= 7,
			&COLOR_RED,		   // O			= 8,
			&COLOR_MAGENTA,	   // F			= 9,
			&COLOR_MAGENTA,	   // NE		= 10,
			&COLOR_VIOLET,	   // NA		= 11,
			&COLOR_GREEN_DARK, // MG		= 12,
			&COLOR_MAGENTA,	   // AL		= 13,
			&COLOR_MAGENTA,	   // SI		= 14,
			&COLOR_ORANGE,	   // P			= 15,
			&COLOR_YELLOW,	   // S			= 16,
			&COLOR_MAGENTA,	   // CL		= 17,
			&COLOR_MAGENTA,	   // AR		= 18,
			&COLOR_MAGENTA,	   // K			= 19,
			&COLOR_MAGENTA,	   // CA		= 20,
			&COLOR_MAGENTA,	   // SC		= 21,
			&COLOR_MAGENTA,	   // TI		= 22,
			&COLOR_MAGENTA,	   // V			= 23,
			&COLOR_MAGENTA,	   // CR		= 24,
			&COLOR_MAGENTA,	   // MN		= 25,
			&COLOR_MAGENTA,	   // FE		= 26,
			&COLOR_MAGENTA,	   // CO		= 27,
			&COLOR_MAGENTA,	   // NI		= 28,
			&COLOR_MAGENTA,	   // CU		= 29,
			&COLOR_MAGENTA,	   // ZN		= 30,
			&COLOR_MAGENTA,	   // GA		= 31,
			&COLOR_MAGENTA,	   // GE		= 32,
			&COLOR_MAGENTA,	   // AS		= 33,
			&COLOR_MAGENTA,	   // SE		= 34,
			&COLOR_MAGENTA,	   // BR		= 35,
			&COLOR_MAGENTA,	   // KR		= 36,
			&COLOR_MAGENTA,	   // RB		= 37,
			&COLOR_MAGENTA,	   // SR		= 38,
			&COLOR_MAGENTA,	   // Y			= 39,
			&COLOR_MAGENTA,	   // ZR		= 40,
			&COLOR_MAGENTA,	   // NB		= 41,
			&COLOR_MAGENTA,	   // MO		= 42,
			&COLOR_MAGENTA,	   // TC		= 43,
			&COLOR_MAGENTA,	   // RU		= 44,
			&COLOR_MAGENTA,	   // RH		= 45,
			&COLOR_MAGENTA,	   // PD		= 46,
			&COLOR_MAGENTA,	   // AG		= 47,
			&COLOR_MAGENTA,	   // CD		= 48,
			&COLOR_MAGENTA,	   // IN		= 49,
			&COLOR_MAGENTA,	   // SN		= 50,
			&COLOR_MAGENTA,	   // SB		= 51,
			&COLOR_MAGENTA,	   // TE		= 52,
			&COLOR_MAGENTA,	   // I			= 53,
			&COLOR_MAGENTA,	   // XE		= 54,
			&COLOR_MAGENTA,	   // CS		= 55,
			&COLOR_MAGENTA,	   // BA		= 56,
			&COLOR_MAGENTA,	   // LA		= 57,
			&COLOR_MAGENTA,	   // CE		= 58,
			&COLOR_MAGENTA,	   // PR		= 59,
			&COLOR_MAGENTA,	   // ND		= 60,
			&COLOR_MAGENTA,	   // PM		= 61,
			&COLOR_MAGENTA,	   // SM		= 62,
			&COLOR_MAGENTA,	   // EU		= 63,
			&COLOR_MAGENTA,	   // GD		= 64,
			&COLOR_MAGENTA,	   // TB		= 65,
			&COLOR_MAGENTA,	   // DY		= 66,
			&COLOR_MAGENTA,	   // HO		= 67,
			&COLOR_MAGENTA,	   // ER		= 68,
			&COLOR_MAGENTA,	   // TM		= 69,
			&COLOR_MAGENTA,	   // YB		= 70,
			&COLOR_MAGENTA,	   // LU		= 71,
			&COLOR_MAGENTA,	   // HF		= 72,
			&COLOR_MAGENTA,	   // TA		= 73,
			&COLOR_MAGENTA,	   // W			= 74,
			&COLOR_MAGENTA,	   // RE		= 75,
			&COLOR_MAGENTA,	   // OS		= 76,
			&COLOR_MAGENTA,	   // IR		= 77,
			&COLOR_MAGENTA,	   // PT		= 78,
			&COLOR_MAGENTA,	   // AU		= 79,
			&COLOR_MAGENTA,	   // HG		= 80,
			&COLOR_MAGENTA,	   // TL		= 81,
			&COLOR_MAGENTA,	   // PB		= 82,
			&COLOR_MAGENTA,	   // BI		= 83,
			&COLOR_MAGENTA,	   // PO		= 84,
			&COLOR_MAGENTA,	   // AT		= 85,
			&COLOR_MAGENTA,	   // RN		= 86,
			&COLOR_MAGENTA,	   // FR		= 87,
			&COLOR_MAGENTA,	   // RA		= 88,
			&COLOR_MAGENTA,	   // AC		= 89,
			&COLOR_MAGENTA,	   // TH		= 90,
			&COLOR_MAGENTA,	   // PA		= 91,
			&COLOR_MAGENTA,	   // U			= 92,
			&COLOR_MAGENTA,	   // NP		= 93,
			&COLOR_MAGENTA,	   // PU		= 94,
			&COLOR_MAGENTA,	   // AM		= 95,
			&COLOR_MAGENTA,	   // CM		= 96,
			&COLOR_MAGENTA,	   // BK		= 97,
			&COLOR_MAGENTA,	   // CF		= 98,
			&COLOR_MAGENTA,	   // ES		= 99,
			&COLOR_MAGENTA,	   // FM		= 100,
			&COLOR_MAGENTA,	   // MD		= 101,
			&COLOR_MAGENTA,	   // NO		= 102,
			&COLOR_MAGENTA,	   // LR		= 103,
			&COLOR_MAGENTA,	   // RF		= 104,
			&COLOR_MAGENTA,	   // DD		= 105,
			&COLOR_MAGENTA,	   // SG		= 106,
			&COLOR_MAGENTA,	   // BHJ		= 107,
			&COLOR_MAGENTA,	   // HS		= 108,
			&COLOR_MAGENTA,	   // MT		= 109,
			&COLOR_MAGENTA,	   // DS		= 110,
			&COLOR_MAGENTA,	   // RG		= 111,
			&COLOR_MAGENTA,	   // UUB		= 112,
			&COLOR_MAGENTA,	   // UUT		= 113,
			&COLOR_MAGENTA,	   // UUQ		= 114,
			&COLOR_MAGENTA,	   // UUP		= 115,
			&COLOR_MAGENTA,	   // UUH		= 116,
			&COLOR_MAGENTA,	   // UUS		= 117,
			&COLOR_MAGENTA	   // UUO		= 118,
		};
	} // namespace Model
} // namespace VTX
