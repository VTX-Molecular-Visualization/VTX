#ifndef __VTX_CORE_STRUCT_MESH__
#define __VTX_CORE_STRUCT_MESH__

#include <util/types.hpp>
#include <vector>

namespace VTX::Core::Struct
{

	struct Mesh
	{
		std::vector<Vec3f> vertices;
		std::vector<Vec3f> normals;
		std::vector<Index> indices;

		void computeNormals();
		void toIndexed();
		void toNonIndexed();
	};

} // namespace VTX::Core::Struct

#endif
