#ifndef __VTX_CORE_CHEMDB_CHAIN__
#define __VTX_CORE_CHEMDB_CHAIN__

#include <string>
#include <util/constants.hpp>

namespace VTX::Core::ChemDB::Chain
{
	// Never used
	// enum class CHAIN_TYPE : int
	//{
	//	STANDARD,
	//	NON_STANDARD
	//};

	const std::string_view NAME[ 53 ] = {
		"--",  "Aa",  "Bb",	 "Cc",	"Dd",  "Ee",  "Ff",	 "Gg",	"Hh",  "Ii",  "Jj",	 "Kk",	"Ll", "Mm",
		"Nn",  "Oo",  "Pp0", "Qq1", "Rr2", "Ss3", "Tt4", "Uu5", "Vv6", "Ww7", "Xx8", "Yy9", "Zz", "Aa",
		"Bb",  "Cc",  "Dd",	 "Ee",	"Ff",  "Gg",  "Hh",	 "Ii",	"Jj",  "Kk",  "Ll",	 "Mm",	"Nn", "Oo",
		"Pp0", "Qq1", "Rr2", "Ss3", "Tt4", "Uu5", "Vv6", "Ww7", "Xx8", "Yy9", "Zz",
	};

} // namespace VTX::Core::ChemDB::Chain

#endif
