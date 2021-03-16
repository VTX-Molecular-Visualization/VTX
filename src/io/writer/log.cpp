#include "log.hpp"
#include "util/filesystem.hpp"

namespace VTX
{
	namespace IO
	{
		namespace Writer
		{
			Log::Log( const std::string & p_fileName )
			{
				_os = std::ofstream( Util::Filesystem::getLogsPath( p_fileName + ".log" ) );
			}

			Log::~Log() { _os.close(); }

			void Log::writeFile( const Path & p_path, const std::string & p_data ) { _os << p_data << std::endl; }

		} // namespace Writer
	}	  // namespace IO
} // namespace VTX
