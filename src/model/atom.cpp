#include "atom.hpp"
#include "view/ui/atom.hpp"

namespace VTX
{
	namespace Model
	{
		void Atom::setSelected( const bool p_selected )
		{
			BaseModel::setSelected( p_selected );
			if ( isSelected() )
			{ addItem( (View::BaseView<BaseModel> *)Generic::create<Atom, View::UI::Atom>( this ) ); }
			else
			{
				Generic::destroy( removeItem( ID::View::UI_ATOM ) );
			}
		}

		const std::string Atom::SYMBOL_STR[ (int)ATOM_SYMBOL::COUNT ] = {
			"UNKNOWN", // UNKNOWN = 0,
			"H",	   // H			= 1,
			"HE",	   // HE		= 2,
			"LI",	   // LI		= 3,
			"BE",	   // BE		= 4,
			"B",	   // B			= 5,
			"C",	   // C			= 6,
			"N",	   // N			= 7,
			"O",	   // O			= 8,
			"F",	   // F			= 9,
			"NE",	   // NE		= 10,
			"NA",	   // NA		= 11,
			"MG",	   // MG		= 12,
			"AL",	   // AL		= 13,
			"SI",	   // SI		= 14,
			"P",	   // P			= 15,
			"S",	   // S			= 16,
			"CL",	   // CL		= 17,
			"AR",	   // AR		= 18,
			"K",	   // K			= 19,
			"CA",	   // CA		= 20,
			"SC",	   // SC		= 21,
			"TI",	   // TI		= 22,
			"V",	   // V			= 23,
			"CR",	   // CR		= 24,
			"MN",	   // MN		= 25,
			"FE",	   // FE		= 26,
			"CO",	   // CO		= 27,
			"NI",	   // NI		= 28,
			"CU",	   // CU		= 29,
			"ZN",	   // ZN		= 30,
			"GA",	   // GA		= 31,
			"GE",	   // GE		= 32,
			"AS",	   // AS		= 33,
			"SE",	   // SE		= 34,
			"BR",	   // BR		= 35,
			"KR",	   // KR		= 36,
			"RB",	   // RB		= 37,
			"SR",	   // SR		= 38,
			"Y",	   // Y			= 39,
			"ZR",	   // ZR		= 40,
			"NB",	   // NB		= 41,
			"MO",	   // MO		= 42,
			"TC",	   // TC		= 43,
			"RU",	   // RU		= 44,
			"RH",	   // RH		= 45,
			"PD",	   // PD		= 46,
			"AG",	   // AG		= 47,
			"CD",	   // CD		= 48,
			"IN",	   // IN		= 49,
			"SN",	   // SN		= 50,
			"SB",	   // SB		= 51,
			"TE",	   // TE		= 52,
			"I",	   // I			= 53,
			"XE",	   // XE		= 54,
			"CS",	   // CS		= 55,
			"BA",	   // BA		= 56,
			"LA",	   // LA		= 57,
			"CE",	   // CE		= 58,
			"PR",	   // PR		= 59,
			"ND",	   // ND		= 60,
			"PM",	   // PM		= 61,
			"SM",	   // SM		= 62,
			"EU",	   // EU		= 63,
			"GD",	   // GD		= 64,
			"TB",	   // TB		= 65,
			"DY",	   // DY		= 66,
			"HO",	   // HO		= 67,
			"ER",	   // ER		= 68,
			"TM",	   // TM		= 69,
			"YB",	   // YB		= 70,
			"LU",	   // LU		= 71,
			"HF",	   // HF		= 72,
			"TA",	   // TA		= 73,
			"W",	   // W			= 74,
			"RE",	   // RE		= 75,
			"OS",	   // OS		= 76,
			"IR",	   // IR		= 77,
			"PT",	   // PT		= 78,
			"AU",	   // AU		= 79,
			"HG",	   // HG		= 80,
			"TL",	   // TL		= 81,
			"PB",	   // PB		= 82,
			"BI",	   // BI		= 83,
			"PO",	   // PO		= 84,
			"AT",	   // AT		= 85,
			"RN",	   // RN		= 86,
			"FR",	   // FR		= 87,
			"RA",	   // RA		= 88,
			"AC",	   // AC		= 89,
			"TH",	   // TH		= 90,
			"PA",	   // PA		= 91,
			"U",	   // U			= 92,
			"NP",	   // NP		= 93,
			"PU",	   // PU		= 94,
			"AM",	   // AM		= 95,
			"CM",	   // CM		= 96,
			"BK",	   // BK		= 97,
			"CF",	   // CF		= 98,
			"ES",	   // ES		= 99,
			"FM",	   // FM		= 100,
			"MD",	   // MD		= 101,
			"NO",	   // NO		= 102,
			"LR",	   // LR		= 103,
			"RF",	   // RF		= 104,
			"DD",	   // DD		= 105,
			"SG",	   // SG		= 106,
			"BHJ",	   // BHJ		= 107,
			"HS",	   // HS		= 108,
			"MT",	   // MT		= 109,
			"DS",	   // DS		= 110,
			"RG",	   // RG		= 111,
			"UUB",	   // UUB		= 112,
			"UUT",	   // UUT		= 113,
			"UUQ",	   // UUQ		= 114,
			"UUP",	   // UUP		= 115,
			"UUH",	   // UUH		= 116,
			"UUS",	   // UUS		= 117,
			"UUO"	   // UUO		= 118,
		};

		const std::string Atom::SYMBOL_NAME[ (int)ATOM_SYMBOL::COUNT ] = {
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

		const float Atom::SYMBOL_VDW_RADIUS[ (int)ATOM_SYMBOL::COUNT ] = {
			1.20f, // UNKNOWN	= 0,
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

		// http://jmol.sourceforge.net/jscolors/#Jmolcolors
		Color Atom::SYMBOL_COLOR[ (int)ATOM_SYMBOL::COUNT ] = {
			{ 255, 255, 255 }, // UNKNOWN		= 0,
			{ 255, 255, 255 }, // H				= 1,
			{ 217, 255, 255 }, // HE			= 2,
			{ 204, 128, 255 }, // LI			= 3,
			{ 194, 255, 0 },   // BE			= 4,
			{ 255, 181, 181 }, // B				= 5,
			{ 144, 144, 144 }, // C				= 6,
			{ 48, 80, 248 },   // N				= 7,
			{ 255, 13, 13 },   // O				= 8,
			{ 144, 224, 80 },  // F				= 9,
			{ 179, 227, 245 }, // NE			= 10,
			{ 171, 92, 242 },  // NA			= 11,
			{ 138, 255, 0 },   // MG			= 12,
			{ 191, 166, 166 }, // AL			= 13,
			{ 240, 200, 160 }, // SI			= 14,
			{ 255, 128, 0 },   // P				= 15,
			{ 255, 255, 48 },  // S				= 16,
			{ 31, 240, 31 },   // CL			= 17,
			{ 128, 209, 227 }, // AR			= 18,
			{ 143, 64, 212 },  // K				= 19,
			{ 61, 255, 0 },	   // CA			= 20,
			{ 230, 230, 230 }, // SC			= 21,
			{ 191, 194, 199 }, // TI			= 22,
			{ 166, 166, 171 }, // V				= 23,
			{ 138, 153, 199 }, // CR			= 24,
			{ 156, 122, 199 }, // MN			= 25,
			{ 224, 102, 51 },  // FE			= 26,
			{ 240, 144, 160 }, // CO			= 27,
			{ 80, 208, 80 },   // NI			= 28,
			{ 200, 128, 51 },  // CU			= 29,
			{ 125, 128, 176 }, // ZN			= 30,
			{ 194, 143, 143 }, // GA			= 31,
			{ 102, 143, 143 }, // GE			= 32,
			{ 189, 128, 227 }, // AS			= 33,
			{ 255, 161, 0 },   // SE			= 34,
			{ 166, 41, 41 },   // BR			= 35,
			{ 92, 184, 209 },  // KR			= 36,
			{ 112, 46, 176 },  // RB			= 37,
			{ 0, 255, 0 },	   // SR			= 38,
			{ 148, 255, 255 }, // Y				= 39,
			{ 148, 224, 224 }, // ZR			= 40,
			{ 115, 194, 201 }, // NB			= 41,
			{ 84, 181, 181 },  // MO			= 42,
			{ 59, 158, 158 },  // TC			= 43,
			{ 36, 143, 143 },  // RU			= 44,
			{ 10, 125, 140 },  // RH			= 45,
			{ 0, 105, 133 },   // PD			= 46,
			{ 192, 192, 192 }, // AG			= 47,
			{ 255, 217, 143 }, // CD			= 48,
			{ 166, 117, 115 }, // IN			= 49,
			{ 102, 128, 128 }, // SN			= 50,
			{ 158, 99, 181 },  // SB			= 51,
			{ 212, 122, 0 },   // TE			= 52,
			{ 148, 0, 148 },   // I				= 53,
			{ 66, 158, 176 },  // XE			= 54,
			{ 87, 23, 143 },   // CS			= 55,
			{ 0, 201, 0 },	   // BA			= 56,
			{ 112, 212, 255 }, // LA			= 57,
			{ 255, 255, 199 }, // CE			= 58,
			{ 217, 255, 199 }, // PR			= 59,
			{ 199, 255, 199 }, // ND			= 60,
			{ 163, 255, 199 }, // PM			= 61,
			{ 143, 255, 199 }, // SM			= 62,
			{ 97, 255, 199 },  // EU			= 63,
			{ 69, 255, 199 },  // GD			= 64,
			{ 48, 255, 199 },  // TB			= 65,
			{ 31, 255, 199 },  // DY			= 66,
			{ 0, 255, 156 },   // HO			= 67,
			{ 0, 230, 117 },   // ER			= 68,
			{ 0, 212, 82 },	   // TM			= 69,
			{ 0, 191, 56 },	   // YB			= 70,
			{ 0, 171, 36 },	   // LU			= 71,
			{ 77, 194, 255 },  // HF			= 72,
			{ 77, 166, 255 },  // TA			= 73,
			{ 33, 148, 214 },  // W				= 74,
			{ 38, 125, 171 },  // RE			= 75,
			{ 38, 102, 150 },  // OS			= 76,
			{ 23, 84, 135 },   // IR			= 77,
			{ 208, 208, 224 }, // PT			= 78,
			{ 255, 209, 35 },  // AU			= 79,
			{ 184, 184, 208 }, // HG			= 80,
			{ 166, 84, 77 },   // TL			= 81,
			{ 87, 89, 97 },	   // PB			= 82,
			{ 158, 79, 181 },  // BI			= 83,
			{ 171, 92, 0 },	   // PO			= 84,
			{ 117, 79, 69 },   // AT			= 85,
			{ 66, 130, 150 },  // RN			= 86,
			{ 66, 0, 102 },	   // FR			= 87,
			{ 0, 125, 0 },	   // RA			= 88,
			{ 112, 171, 250 }, // AC			= 89,
			{ 0, 186, 255 },   // TH			= 90,
			{ 0, 161, 255 },   // PA			= 91,
			{ 0, 143, 255 },   // U				= 92,
			{ 0, 128, 255 },   // NP			= 93,
			{ 0, 107, 255 },   // PU			= 94,
			{ 84, 92, 242 },   // AM			= 95,
			{ 120, 92, 227 },  // CM			= 96,
			{ 138, 79, 227 },  // BK			= 97,
			{ 161, 54, 212 },  // CF			= 98,
			{ 179, 31, 212 },  // ES			= 99,
			{ 179, 31, 186 },  // FM			= 100,
			{ 179, 13, 16 },   // MD			= 101,
			{ 189, 13, 135 },  // NO			= 102,
			{ 199, 0, 102 },   // LR			= 103,
			{ 204, 0, 89 },	   // RF			= 104,
			{ 209, 0, 79 },	   // DD			= 105,
			{ 217, 0, 69 },	   // SG			= 106,
			{ 224, 0, 56 },	   // BHJ			= 107,
			{ 230, 0, 46 },	   // HS			= 108,
			{ 235, 0, 38 },	   // MT			= 109,
			{ 255, 255, 255 }, // DS			= 110,
			{ 255, 255, 255 }, // RG			= 111,
			{ 255, 255, 255 }, // UUB			= 112,
			{ 255, 255, 255 }, // UUT			= 113,
			{ 255, 255, 255 }, // UUQ			= 114,
			{ 255, 255, 255 }, // UUP			= 115,
			{ 255, 255, 255 }, // UUH			= 116,
			{ 255, 255, 255 }, // UUS			= 117,
			{ 255, 255, 255 }  // UUO			= 118,
		};
	} // namespace Model
} // namespace VTX
