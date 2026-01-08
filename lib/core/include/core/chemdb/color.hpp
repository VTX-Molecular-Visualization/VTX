#ifndef __VTX_CORE_CHEMDB_COLOR__
#define __VTX_CORE_CHEMDB_COLOR__

namespace VTX::Core::ChemDB::Atom
{
	enum struct COLOR_SCHEME : int
	{
		MOLECULE,
		CHAIN,
		RESIDUE,
		ATOM,
		AND_MORE
	};
}

#endif
