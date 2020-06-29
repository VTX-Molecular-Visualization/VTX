#include "vtx.hpp"

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			void VTX::readFile( const Path & p_path, VTXApp & p_data )
			{
				std::ifstream  is( p_path );
				nlohmann::json json;
				is >> json;
				p_data.fromJson( json );
			}
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
