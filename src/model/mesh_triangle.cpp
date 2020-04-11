#include "mesh_triangle.hpp"
#include "util/logger.hpp"

namespace VTX
{
	namespace Model
	{
	
		void MeshTriangle::print() const
		{
			VTX_INFO( "Faces: " + std::to_string( _triangles.size() ) );
			VTX_INFO( "Vertices: " + std::to_string( _vertices.size() ) );
		 }

	} // namespace Model
} // namespace VTX
