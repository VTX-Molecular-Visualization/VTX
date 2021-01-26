#include "vtx.hpp"
#include "io/serializer.hpp"
#include "util/filesystem.hpp"
#include <nlohmann/json.hpp>

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			void VTX::readFile( const FilePath & p_path, VTXApp & p_app )
			{
				IO::Serializer serializer = IO::Serializer();
				std::ifstream  ifs( p_path );
				nlohmann::json json;
				ifs >> json;
				serializer.deserialize( json, p_app );
			}
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
