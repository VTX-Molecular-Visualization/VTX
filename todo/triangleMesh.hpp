#ifndef __ISICG_ISIR_TRIANGLE_MESH__
#define __ISICG_ISIR_TRIANGLE_MESH__

#include <vector>

#include "assimp/mesh.h"

#include "defines.hpp"

namespace ISICG_ISIR
{
	struct MeshTriangle
	{
		// vertices ids in TriangleMesh::_vertices
		union
		{
			struct
			{
				uint _v0, _v1, _v2;
			};
			uint _v[3];
		};
	};

	class TriangleMesh
	{
	public:
		TriangleMesh() = default;

		void load(const std::string &path);

	private:
		std::vector<MeshTriangle> _triangles;
		std::vector<Vec3f> _vertices;
	};
} // namespace ISICG_ISIR

#endif // __ISICG_ISIR_TRIANGLE_MESH__
