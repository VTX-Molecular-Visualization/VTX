#pragma once

#include <cstdint>

namespace VTX::IO::Writer
{
	enum class E_BOND_ORDER : uint8_t
	{
		unknown,
		single,
		doubl,
		triple,
		quadruple,
		quintuple,
		down,
		up,
		dative_L,
		dative_R,
		amide,
		aromatic,
	};

	/**
	 * @brief Helps to know what type of structural data to write. In the future we will probably support docking
	 * results for instance
	 */
	enum class E_WRITE_TYPE : uint8_t
	{
		trajectory
	};
} // namespace VTX::IO::Writer
