#ifndef __LANGUAGE__
#define __LANGUAGE__

#include "lang_en.hpp"
#include <iostream>
#include <map>

#define LOCALE( s ) VTX::Localization::Language::text( s )

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
				catch ( std::exception p_e )
				{
					return p_key;
				}
			};
		};
	} // namespace Localization
} // namespace VTX
#endif
