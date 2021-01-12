#ifndef __VTX_SCENE__
#define __VTX_SCENE__

#ifdef _MSC_VER
#pragma once
#endif

#include "camera.hpp"
#include "generic/base_updatable.hpp"
#include "model/mesh_triangle.hpp"
#include "model/molecule.hpp"
#include "model/path.hpp"
#include <vector>

namespace VTX
{
	namespace Object3D
	{
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
			~Scene() { clear(); }

			inline Camera &						 getCamera() { return _camera; }
			inline const Camera &				 getCamera() const { return _camera; }
			inline MapMoleculePtrFloat &		 getMolecules() { return _molecules; };
			inline const MapMoleculePtrFloat &	 getMolecules() const { return _molecules; };
			inline const VectorPathPtr &		 getPaths() const { return _paths; };
			inline const VectorMeshTrianglePtr & getMeshes() const { return _meshes; };
			inline const Math::AABB &			 getAABB() const { return _aabb; }

			void addMolecule( MoleculePtr const );
			void removeMolecule( MoleculePtr const );
			void addPath( PathPtr const );
			void addMesh( MeshTrianglePtr const );
			void removeMesh( MeshTrianglePtr const );

			void clear();

			virtual void update( const float & ) override;

		  private:
			void _computeAABB();

		  private:
			Camera				  _camera = Camera();
			Math::AABB			  _aabb;
			MapMoleculePtrFloat	  _molecules = MapMoleculePtrFloat();
			VectorPathPtr		  _paths	 = VectorPathPtr();
			VectorMeshTrianglePtr _meshes	 = VectorMeshTrianglePtr();
		};
	} // namespace Object3D
} // namespace VTX
#endif
