#ifndef __VTX_SCENE__
#define __VTX_SCENE__

#include "generic/base_updatable.hpp"
#include "math/aabb.hpp"
#include <map>
#include <vector>

namespace VTX::Model
{
	class MeshTriangle;
	class Molecule;
	class Path;
} // namespace VTX::Model

namespace VTX::Object3D
{
	class Camera;
	class Scene : public Generic::BaseUpdatable
	{
	  public:
		using MoleculePtr			= Model::Molecule *;
		using PathPtr				= Model::Path *;
		using MeshTrianglePtr		= Model::MeshTriangle *;
		using MapMoleculePtrFloat	= std::map<MoleculePtr, float>;
		using PairMoleculePtrFloat	= std::pair<const MoleculePtr, float>;
		using VectorPathPtr			= std::vector<PathPtr>;
		using VectorMeshTrianglePtr = std::vector<MeshTrianglePtr>;

		Scene();
		~Scene();

		inline Camera &						 getCamera() { return *_camera; }
		inline const Camera &				 getCamera() const { return *_camera; }
		inline MapMoleculePtrFloat &		 getMolecules() { return _molecules; };
		inline const MapMoleculePtrFloat &	 getMolecules() const { return _molecules; };
		inline const VectorPathPtr &		 getPaths() const { return _paths; };
		inline const VectorMeshTrianglePtr & getMeshes() const { return _meshes; };
		const Math::AABB &					 getAABB();

		void addMolecule( MoleculePtr const, const bool p_sendEvent = true );
		void removeMolecule( MoleculePtr const );
		void addPath( PathPtr const );
		void addMesh( MeshTrianglePtr const );
		void removeMesh( MeshTrianglePtr const );

		bool isEmpty() const;

		void clear();
		void reset();

		virtual void update( const float & ) override;

	  private:
		void _computeAABB();
		void _createDefaultPath();

	  private:
		Camera *			  _camera = nullptr;
		Math::AABB			  _aabb;
		MapMoleculePtrFloat	  _molecules = MapMoleculePtrFloat();
		VectorPathPtr		  _paths	 = VectorPathPtr();
		VectorMeshTrianglePtr _meshes	 = VectorMeshTrianglePtr();
	};
} // namespace VTX::Object3D
#endif
