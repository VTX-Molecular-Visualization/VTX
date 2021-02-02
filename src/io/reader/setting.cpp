#include "setting.hpp"
#include "io/serializer.hpp"
#include "util/filesystem.hpp"

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			void Setting::readFile( const FilePath & p_path, ::VTX::Setting & p_setting )
			{
				IO::Serializer serializer = IO::Serializer();
				std::ifstream  is( p_path );
				nlohmann::json json;
				is >> json;
				serializer.deserialize( json, p_setting );
			}
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
