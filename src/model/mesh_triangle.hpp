#ifndef __MODEL_MESH_TRIANGLE__
#define __MODEL_MESH_TRIANGLE__

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
			VTX_MODEL

		  public:
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

			inline const std::vector<uint> & getVisibilities() const { return _visibilities; }
			inline std::vector<uint> &		 getVisibilities() { return _visibilities; }
			inline const uint &				 getVisibilitie( const uint p_idx ) const { return _visibilities[ p_idx ]; }
			inline uint &					 getVisibilitie( const uint p_idx ) { return _visibilities[ p_idx ]; }

			inline const std::vector<uint> & getIndices() const { return _indices; }
			inline std::vector<uint> &		 getIndices() { return _indices; }
			inline const uint &				 getIndice( const uint p_idx ) const { return _indices[ p_idx ]; }
			inline uint &					 getIndice( const uint p_idx ) { return _indices[ p_idx ]; }

			void print() const;

		  private:
			std::vector<Vec3f>		_vertices;
			std::vector<Vec3f>		_normals;
			std::vector<Color::Rgb> _colors;
			std::vector<uint>		_visibilities;
			std::vector<uint>		_indices;

			MeshTriangle();
			~MeshTriangle() = default;

			void _computeAABB() const override;
			void _fillBuffer() override;
			void _instantiate3DViews() override;
		};
	} // namespace Model
} // namespace VTX

#endif
