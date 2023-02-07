#include "writer_exception.hpp"
#include "util/filesystem.hpp"
#include <fstream>
#include <iostream>
#include <string>

namespace VTX
{
	namespace IO
	{
		namespace Writer
		{
			void Exception::writeFile( const Util::FilePath & p_path, const std::exception & p_exception )
			{
				std::ofstream os = std::ofstream( p_path.path() );
				os << p_exception.what() << std::endl;
				os.close();
			}

		} // namespace Writer
	}	  // namespace IO
} // namespace VTX
