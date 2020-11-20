#ifndef __MODEL_MESH_TRIANGLE__
#define __MODEL_MESH_TRIANGLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_model_3d.hpp"
#include "buffer/mesh_triangle.hpp"
#include "color/rgb.hpp"
#include "define.hpp"
#include <vector>

namespace VTX
{
	namespace Model
	{
		class MeshTriangle : public BaseModel3D<Buffer::MeshTriangle>
		{
		  public:
			MeshTriangle();
			~MeshTriangle();

			void instantiateDefaultViews();

			inline const std::vector<Vec3f> & getVertices() const { return _vertices; }
			inline std::vector<Vec3f> &		  getVertices() { return _vertices; }
			inline const Vec3f &			  getVertice( const uint p_idx ) const { return _vertices[ p_idx ]; }
			inline Vec3f &					  getVertice( const uint p_idx ) { return _vertices[ p_idx ]; }

			inline const std::vector<Vec3f> & getNormals() const { return _normals; }
			inline std::vector<Vec3f> &		  getNormals() { return _normals; }
			inline const Vec3f &			  getNormal( const uint p_idx ) const { return _normals[ p_idx ]; }
			inline Vec3f &					  getNormal( const uint p_idx ) { return _normals[ p_idx ]; }

			inline const std::vector<Color::Rgb> & getColors() const { return _colors; }
			inline std::vector<Color::Rgb> &	   getColors() { return _colors; }
			inline const Color::Rgb &			   getColor( const uint p_idx ) const { return _colors[ p_idx ]; }
			inline Color::Rgb &					   getColor( const uint p_idx ) { return _colors[ p_idx ]; }

			inline const std::vector<ushort> & getVisibilities() const { return _visibilities; }
			inline std::vector<ushort> &	   getVisibilities() { return _visibilities; }
			inline const ushort &			   getVisibilitie( const uint p_idx ) const { return _visibilities[ p_idx ]; }
			inline ushort &					   getVisibilitie( const uint p_idx ) { return _visibilities[ p_idx ]; }

			inline const std::vector<uint> & getIndices() const { return _indices; }
			inline std::vector<uint> &		 getIndices() { return _indices; }
			inline const uint &				 getIndice( const uint p_idx ) const { return _indices[ p_idx ]; }
			inline uint &					 getIndice( const uint p_idx ) { return _indices[ p_idx ]; }

			void print() const;

		  protected:
			std::vector<Vec3f>		_vertices;
			std::vector<Vec3f>		_normals;
			std::vector<Color::Rgb> _colors;
			std::vector<ushort>		_visibilities;
			std::vector<uint>		_indices;

			void _computeAABB();
			void _init() override;
			void _instanciate3DViews() override;
		};
	} // namespace Model
} // namespace VTX

#endif
