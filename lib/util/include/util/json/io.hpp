#ifndef __VTX_UTIL_JSON_IO__
#define __VTX_UTIL_JSON_IO__

#include "util/filesystem.hpp"
#include "util/json/basic_json.hpp"

namespace VTX::Util::JSon
{
	class IO
	{
	  public:
		static Document open( const FilePath & p_filepath );
		static void		write( const FilePath & p_filepath, const Document & p_json );
	};
} // namespace VTX::Util::JSon
#endif
