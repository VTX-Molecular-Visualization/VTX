#ifndef __MODEL_MESH_TRIANGLE__
#define __MODEL_MESH_TRIANGLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_model_3d.hpp"
#include "define.hpp"
#include <vector>

namespace VTX
{
	namespace Model
	{
		class MeshTriangle : public BaseModel3D
		{
		  public:
			/*
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
			*/

			MeshTriangle() = default;
			~MeshTriangle();

			// const std::vector<Triangle> & getTriangles() const { return _triangles; }
			// std::vector<Triangle> &		  getTriangles() { return _triangles; }
			// const Triangle &			  getTriangle( uint p_idx ) const { return _triangles[ p_idx ]; }
			// Triangle &					  getTriangle( uint p_idx ) { return _triangles[ p_idx ]; }

			inline const std::vector<Vec3f> & getVertices() const { return _vertices; }
			inline std::vector<Vec3f> &		  getVertices() { return _vertices; }
			inline const Vec3f &			  getVertice( uint p_idx ) const { return _vertices[ p_idx ]; }
			inline Vec3f &					  getVertice( uint p_idx ) { return _vertices[ p_idx ]; }

			inline const std::vector<Vec3f> & getNormals() const { return _normals; }
			inline std::vector<Vec3f> &		  getNormals() { return _normals; }
			inline const Vec3f &			  getNormal( uint p_idx ) const { return _normals[ p_idx ]; }
			inline Vec3f &					  getNormal( uint p_idx ) { return _normals[ p_idx ]; }

			inline const std::vector<Vec3f> & getColors() const { return _colors; }
			inline std::vector<Vec3f> &		  getColors() { return _colors; }
			inline const Vec3f &			  getColor( uint p_idx ) const { return _colors[ p_idx ]; }
			inline Vec3f &					  getColor( uint p_idx ) { return _colors[ p_idx ]; }

			inline const std::vector<uint> & getVisibilities() const { return _visibilities; }
			inline std::vector<uint> &		 getVisibilities() { return _visibilities; }
			inline const uint &				 getVisibilitie( uint p_idx ) const { return _visibilities[ p_idx ]; }
			inline uint &					 getVisibilitie( uint p_idx ) { return _visibilities[ p_idx ]; }

			inline const std::vector<uint> & getIndices() const { return _indices; }
			inline std::vector<uint> &		 getIndices() { return _indices; }
			inline const uint &				 getIndice( uint p_idx ) const { return _indices[ p_idx ]; }
			inline uint &					 getIndice( uint p_idx ) { return _indices[ p_idx ]; }

			virtual void print() const override;
			virtual void init() override;
			virtual void bindBuffers() override;
			virtual void unbindBuffers() override;

		  protected:
			virtual void _addItems() override final;

			enum ATTRIBUTE_LOCATION
			{
				VERTEX_POSITION	  = 0,
				VERTEX_NORMAL	  = 1,
				VERTEX_COLOR	  = 2,
				VERTEX_VISIBILITY = 3,
			};

			// std::vector<Triangle> _triangles;
			std::vector<Vec3f> _vertices;
			std::vector<Vec3f> _normals;
			std::vector<Vec3f> _colors;
			std::vector<uint>  _visibilities;

			std::vector<uint> _indices;

			GLuint _vboPositions	= GL_INVALID_VALUE;
			GLuint _vboNormals		= GL_INVALID_VALUE;
			GLuint _vboColors		= GL_INVALID_VALUE;
			GLuint _vboVisibilities = GL_INVALID_VALUE;
			GLuint _ibo				= GL_INVALID_VALUE;
			GLuint _vao				= GL_INVALID_VALUE;
		};
	} // namespace Model
} // namespace VTX

#endif
