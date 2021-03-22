#include "setting.hpp"
#include "io/serializer.hpp"
#include "util/filesystem.hpp"

namespace VTX
{
	namespace IO
	{
		namespace Writer
		{
			void Setting::writeFile( const FilePath & p_path, const ::VTX::Setting & p_setting )
			{
				IO::Serializer serializer = IO::Serializer();
				nlohmann::json json		  = serializer.serialize( p_setting );
				std::ofstream  os( p_path );
				os << std::setw( 4 ) << json << std::endl;
				os.close();
			}
		} // namespace Writer
	}	  // namespace IO
} // namespace VTX
