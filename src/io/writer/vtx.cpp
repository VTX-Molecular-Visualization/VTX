#include "vtx.hpp"
#include "io/serializer.hpp"

namespace VTX
{
	namespace IO
	{
		namespace Writer
		{
			void VTX::writeFile( const Path & p_path, const VTXApp & p_data )
			{
				IO::Serializer serializer = IO::Serializer();
				nlohmann::json json		  = serializer.serialize( p_data );
				std::ofstream  os( p_path );
				os << std::setw( 4 ) << json << std::endl;
			}
		} // namespace Writer
	}	  // namespace IO
} // namespace VTX
