#ifndef __VTX_CORE_CHEMDB_CHAIN__
#define __VTX_CORE_CHEMDB_CHAIN__

#include <util/color/rgba.hpp>

namespace VTX::Core::ChemDB::Chain
{
	// Never used
	// enum class CHAIN_TYPE : int
	//{
	//	STANDARD,
	//	NON_STANDARD
	//};

	// one color per chain id + 1 unknown
	const size_t NB_COLORS = 26;

	const Util::Color::Rgba CHAIN_ID_COLOR_ATOM[ NB_COLORS ] = {
		{ 192, 208, 255 }, // A, a,
		{ 176, 255, 176 }, // B, b,
		{ 255, 192, 200 }, // C, c,
		{ 255, 255, 128 }, // D, d,
		{ 255, 192, 255 }, // E, e,
		{ 176, 240, 240 }, // F, f,
		{ 255, 208, 112 }, // G, g,
		{ 240, 128, 128 }, // H, h,
		{ 245, 222, 179 }, // I, i,
		{ 0, 191, 255 },   // J, j,
		{ 205, 92, 92 },   // K, k,
		{ 102, 205, 170 }, // L, l,
		{ 154, 205, 50 },  // M, m,
		{ 238, 130, 238 }, // N, n,
		{ 0, 206, 209 },   // O, o,
		{ 0, 255, 127 },   // P, p, 0,
		{ 60, 179, 113 },  // Q, q, 1,
		{ 0, 0, 139 },	   // R, r, 2,
		{ 189, 183, 107 }, // S, s, 3,
		{ 0, 100, 0 },	   // T, t, 4,
		{ 128, 0, 0 },	   // U, u, 5,
		{ 128, 128, 0 },   // V, v, 6,
		{ 128, 0, 128 },   // W, w, 7,
		{ 0, 128, 128 },   // X, x, 8,
		{ 184, 134, 11 },  // Y, y, 9,
		{ 178, 34, 34 },   // Z, z
	};

	const Util::Color::Rgba CHAIN_ID_COLOR_HETATM[ NB_COLORS ] = {
		{ 144, 160, 207 }, // A, a,
		{ 128, 207, 152 }, // B, b,
		{ 207, 144, 176 }, // C, c,
		{ 207, 207, 112 }, // D, d,
		{ 207, 144, 207 }, // E, e,
		{ 128, 192, 192 }, // F, f,
		{ 207, 160, 96 },  // G, g,
		{ 192, 80, 112 },  // H, h,
		{ 197, 174, 131 }, // I, i,
		{ 0, 167, 207 },   // J, j,
		{ 181, 76, 76 },   // K, k,
		{ 86, 181, 146 },  // L, l,
		{ 138, 181, 42 },  // M, m,
		{ 190, 114, 190 }, // N, n,
		{ 0, 182, 161 },   // O, o,
		{ 0, 207, 111 },   // P, p, 0,
		{ 52, 155, 97 },   // Q, q, 1,
		{ 0, 0, 187 },	   // R, r, 2,
		{ 165, 159, 91 },  // S, s, 3,
		{ 0, 148, 0 },	   // T, t, 4,
		{ 176, 0, 0 },	   // U, u, 5,
		{ 176, 176, 0 },   // V, v, 6,
		{ 176, 0, 176 },   // W, w, 7,
		{ 0, 176, 176 },   // X, x, 8,
		{ 232, 182, 19 },  // Y, y, 9,
		{ 194, 50, 50 },   // Z, z
	};

	const Util::Color::Rgba CHAIN_ID_UNKNOWN_COLOR = Util::Color::Rgba::WHITE;

} // namespace VTX::Core::ChemDB::Chain

#endif
