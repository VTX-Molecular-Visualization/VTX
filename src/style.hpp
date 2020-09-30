#ifndef __VTX_STYLE__
#define __VTX_STYLE__

#ifdef _MSC_VER
#pragma once
#endif

namespace VTX
{
	namespace Style
	{
		enum class THEME : int
		{
			LIGHT,
			DARK,
			CLASSIC
		};

		enum class SYMBOL_DISPLAY_MODE : int
		{
			SHORT,
			LONG
		};
	} // namespace Style
} // namespace VTX

#endif
