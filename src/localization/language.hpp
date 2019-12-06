#ifndef __VTX_LANGUAGE__
#define __VTX_LANGUAGE__

#ifdef _MSC_VER
#pragma once
#endif

#include "lang_en.hpp"
#include <iostream>
#include <map>

namespace VTX
{
	namespace Localization
	{
		class Language
		{
		  public:
			static const char * text( const char * p_key )
			{
				try
				{
					return values.at( p_key );
				}
				catch ( const std::exception )
				{
					// Use key as text if not translated.
					return p_key;
				}
			};
		};
	} // namespace Localization
} // namespace VTX
#endif
