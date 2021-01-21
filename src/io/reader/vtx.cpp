#include "vtx.hpp"
#include "io/serializer.hpp"

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			void VTX::readFile( const FilePath & p_path, VTXApp & p_app )
			{
				IO::Serializer serializer = IO::Serializer();
				std::ifstream  is( p_path );
				nlohmann::json json;
				is >> json;
				serializer.deserialize( json, p_app );
			}
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
