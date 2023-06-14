#include "app/old/internal/io/writer/writer_exception.hpp"
#include "app/old/internal/io/filesystem.hpp"
#include <fstream>
#include <iostream>
#include <string>

namespace VTX::App::Old::Internal::IO::Writer
{
	void writeFile( const FilePath & p_path, const std::exception & p_exception )
	{
		std::ofstream os = std::ofstream( p_path );
		os << p_exception.what() << std::endl;
		os.close();
	}

} // namespace VTX::App::Old::Internal::IO::Writer
