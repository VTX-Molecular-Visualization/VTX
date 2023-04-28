#ifndef __VTX_APP_COMPONENT_OBJECT3D_MESH_TRIANGLE__
#define __VTX_APP_COMPONENT_OBJECT3D_MESH_TRIANGLE__

#include "app/core/model/base_model_3d.hpp"
#include "app/core/scene/base_scene_item.hpp"
#include "app/old_app/buffer/mesh_triangle.hpp"
#include "util/color/rgba.hpp"
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Component::Object3D
{
	class MeshTriangle : public Core::Model::BaseModel3D<Buffer::MeshTriangle>, public Core::Scene::BaseSceneItem
	{
		VTX_MODEL

	  public:
		// BaseSceneItem
		const Core::Model::ID & getModelID() const override { return getId(); }

		inline const std::vector<Vec4f> & getVertices() const { return _vertices; }
		inline std::vector<Vec4f> &		  getVertices() { return _vertices; }
		inline const Vec4f &			  getVertice( const uint p_idx ) const { return _vertices[ p_idx ]; }
		inline Vec4f &					  getVertice( const uint p_idx ) { return _vertices[ p_idx ]; }

		inline const std::vector<Vec4f> & getNormals() const { return _normals; }
		inline std::vector<Vec4f> &		  getNormals() { return _normals; }
		inline const Vec4f &			  getNormal( const uint p_idx ) const { return _normals[ p_idx ]; }
		inline Vec4f &					  getNormal( const uint p_idx ) { return _normals[ p_idx ]; }

		inline const std::vector<Util::Color::Rgba> & getColors() const { return _colors; }
		inline std::vector<Util::Color::Rgba> &		getColors() { return _colors; }
		inline const Util::Color::Rgba &				getColor( const uint p_idx ) const { return _colors[ p_idx ]; }
		inline Util::Color::Rgba &					getColor( const uint p_idx ) { return _colors[ p_idx ]; }

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

		// CPU.
		static void computeNormals( const std::vector<Vec4f> & p_vertices,
									const std::vector<uint> &  p_indices,
									std::vector<Vec4f> &	   p_normals );
		static void toIndexed( std::vector<Vec4f> & p_vertices, std::vector<uint> & p_indices );
		static void toNonIndexed( std::vector<Vec4f> & p_vertices, std::vector<uint> & p_indices );

		// GPU.
		static void toIndexed();

		void print() const;

	  protected:
		MeshTriangle();
		~MeshTriangle() = default;

		void		 _init() override;
		void		 _computeAABB() const override;
		virtual void _instantiate3DViews() override;

		std::vector<Vec4f>			 _vertices;
		std::vector<Vec4f>			 _normals;
		std::vector<Util::Color::Rgba>	 _colors;
		std::vector<uint>			 _visibilities;
		std::vector<uint>			 _selections;
		std::vector<Core::Model::ID> _ids;
		std::vector<uint>			 _indices;
	};
} // namespace VTX::App::Component::Object3D

#endif
