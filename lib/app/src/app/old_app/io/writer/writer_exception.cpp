#include "app/old_app/io/writer/writer_exception.hpp"
#include "app/old_app/io/filesystem.hpp"
#include <fstream>
#include <iostream>
#include <string>

namespace VTX
{
	namespace IO
	{
		namespace Writer
		{
			void writeFile( const FilePath & p_path, const std::exception & p_exception )
			{
				std::ofstream os = std::ofstream( p_path );
				os << p_exception.what() << std::endl;
				os.close();
			}

		} // namespace Writer
	}	  // namespace IO
} // namespace VTX
