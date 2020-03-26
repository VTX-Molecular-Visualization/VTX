#include "prm.hpp"

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			bool PRM::readFile( const Path & p_path, PRMFile & p_prm ) { return true; }

			bool PRM::readBuffer( const std::string & p_buffer, PRMFile & p_prm ) { return false; }

		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
