#ifndef __MESH_TRIANGLE__
#define __MESH_TRIANGLE__

#include "base_model.hpp"
#include "define.hpp"
#include <vector>

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

			MeshTriangle()	= default;
			~MeshTriangle() = default;

			const std::vector<Triangle> & getTriangles() const { return _triangles; }
			std::vector<Triangle> &		  getTriangles() { return _triangles; }
			const Triangle &			  getTriangle( uint p_idx ) const { return _triangles[ p_idx ]; }
			Triangle &					  getTriangle( uint p_idx ) { return _triangles[ p_idx ]; }

			const std::vector<Vec3f> & getVertices() const { return _vertices; }
			std::vector<Vec3f> &	   getVertices() { return _vertices; }
			const Vec3f &			   getVertice( uint p_idx ) const { return _vertices[ p_idx ]; }
			Vec3f &					   getVertice( uint p_idx ) { return _vertices[ p_idx ]; }

			virtual void print() const override;

		  private:
			std::vector<Triangle> _triangles;
			std::vector<Vec3f>	  _vertices;
		};
	} // namespace Model
} // namespace VTX

#endif
