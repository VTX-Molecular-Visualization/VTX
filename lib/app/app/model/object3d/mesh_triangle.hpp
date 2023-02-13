#ifndef __MODEL_MESH_TRIANGLE__
#define __MODEL_MESH_TRIANGLE__

#include "generic/base_scene_item.hpp"
#include "model/base_model_3d.hpp"
#include "model/color/rgb.hpp"
#include "model/define.hpp"
#include <vector>

namespace VTX
{
	namespace Model
	{
		class MeshTriangle : public BaseModel3D, public Generic::BaseSceneItem
		{
			VTX_MODEL

		  public:
			// BaseSceneItem
			const Model::ID & getModelID() const override { return getId(); }

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

			inline const std::vector<uint> & getSelections() const { return _selections; }
			inline std::vector<uint> &		 getSelections() { return _selections; }
			inline const uint &				 getSelection( const uint p_idx ) const { return _selections[ p_idx ]; }
			inline uint &					 getSelection( const uint p_idx ) { return _selections[ p_idx ]; }

			inline const std::vector<uint> & getIndices() const { return _indices; }
			inline std::vector<uint> &		 getIndices() { return _indices; }
			inline const uint &				 getIndice( const uint p_idx ) const { return _indices[ p_idx ]; }
			inline uint &					 getIndice( const uint p_idx ) { return _indices[ p_idx ]; }

			void recomputeNormals();

			void print() const;

		  protected:
			MeshTriangle();
			~MeshTriangle() = default;

			void _init() override;
			void _computeAABB() const override;

			std::vector<Vec3f>		_vertices;
			std::vector<Vec3f>		_normals;
			std::vector<Color::Rgb> _colors;
			std::vector<uint>		_visibilities;
			std::vector<uint>		_selections;
			std::vector<Model::ID>	_ids;
			std::vector<uint>		_indices;
		};
	} // namespace Model
} // namespace VTX

#endif