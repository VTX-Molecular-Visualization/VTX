#ifndef __VTX_SCENE__
#define __VTX_SCENE__

#ifdef _MSC_VER
#pragma once
#endif

#include "camera.hpp"
#include "generic/base_cleanable.hpp"
#include "generic/base_updatable.hpp"
#include "model/mesh_triangle.hpp"
#include "model/molecule.hpp"
#include "model/path.hpp"
#include <vector>

namespace VTX
{
	namespace Object3D
	{
		class Scene : public Generic::BaseUpdatable, public Generic::BaseCleanable
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

			virtual void clean() override;

			void addMolecule( MoleculePtr const p_molecule ) { _molecules.emplace( p_molecule, 0.f ); }
			void removeMolecule( MoleculePtr const p_molecule ) { _molecules.erase( p_molecule ); }
			void addPath( PathPtr const p_path ) { _paths.emplace_back( p_path ); }

			inline Camera &						 getCamera() { return _camera; }
			inline const Camera &				 getCamera() const { return _camera; }
			inline MapMoleculePtrFloat &		 getMolecules() { return _molecules; };
			inline const MapMoleculePtrFloat &	 getMolecules() const { return _molecules; };
			inline const VectorPathPtr &		 getPaths() const { return _paths; };
			inline const VectorMeshTrianglePtr & getMeshes() const { return _meshes; };

			virtual void update( const double ) override;

		  private:
			Camera				  _camera	 = Camera();
			MapMoleculePtrFloat	  _molecules = MapMoleculePtrFloat();
			VectorPathPtr		  _paths	 = VectorPathPtr();
			VectorMeshTrianglePtr _meshes	 = VectorMeshTrianglePtr();
		};
	} // namespace Object3D
} // namespace VTX
#endif
