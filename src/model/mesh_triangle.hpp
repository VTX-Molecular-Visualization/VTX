#ifndef __MESH_TRIANGLE__
#define __MESH_TRIANGLE__

#include "define.hpp"
#include <vector>
#include "base_model.hpp"

namespace VTX
{
	namespace Model
	{
		class MeshTriangle : public BaseModel
		{
		  public:
			struct Triangle
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

			MeshTriangle() = default;
			~MeshTriangle() = default;

		  private:
			std::vector<Triangle> _triangles;
			std::vector<Vec3f>	  _vertices;
		};
	} // namespace Object3D
} // namespace VTX

#endif
