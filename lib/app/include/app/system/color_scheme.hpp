#ifndef __VTX_APP_SYSTEM_COLOR_SCHEME__
#define __VTX_APP_SYSTEM_COLOR_SCHEME

#include <util/math/range_list.hpp>

namespace VTX::App::System
{
	enum struct COLOR_SCHEME : uint
	{
		MOLECULE,
		CHAIN,
		RESIDUE,
		ATOM,
		AND_MORE
	};

	/**
	 * @brief Store color scheme informations.
	 */
	struct ColorScheme
	{
		int dummy;
	};
} // namespace VTX::App::System

#endif
