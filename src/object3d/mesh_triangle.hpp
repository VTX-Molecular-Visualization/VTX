#ifndef __TRIANGLE_MESH__
#define __TRIANGLE_MESH__

#include "assimp/mesh.h"
#include "define.hpp"
#include <vector>

namespace VTX
{
	namespace Object3D
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
				uint _v[ 3 ];
			};
		};

		class TriangleMesh
		{
		  public:
			TriangleMesh() = default;

		  private:
			std::vector<MeshTriangle> _triangles;
			std::vector<Vec3f>		  _vertices;
		};
	} // namespace Object3D
} // namespace VTX

#endif
