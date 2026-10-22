#ifndef __VTX_IO_WRITER_SHARED__
#define __VTX_IO_WRITER_SHARED__

#include <cstdint>

namespace VTX::IO::Writer
{
	enum class E_FILE_FORMATS : uint8_t
	{
		none,
		pdb,
		mmcif,
		mmtf,
		molden,
		cif,
		gro,
		mol2,
		sdf,
		smi,
		xyz,
		cml,
		cssr,
		nc,
		dcd,
		lammpstrj,
		arc,
		trr,
		xtc,
		tng,
		trj,
		COUNT
	};

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

#endif
