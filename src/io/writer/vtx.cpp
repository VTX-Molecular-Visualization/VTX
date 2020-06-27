#include "vtx.hpp"
#include "util/filesystem.hpp"

namespace VTX
{
	namespace IO
	{
		namespace Writer
		{
			void VTX::writeFile( const Path & p_path, const Object3D::Scene & p_data )
			{
				/*
				nlohmann::json json = p_data.toJson();
				std::ofstream  os( p_path );
				os << std::setw( 4 ) << json << std::endl;
				*/
			}
		} // namespace Writer
	}	  // namespace IO
} // namespace VTX
