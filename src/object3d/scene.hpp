#ifndef __VTX_SCENE__
#define __VTX_SCENE__

#ifdef _MSC_VER
#pragma once
#endif

#include "camera.hpp"
#include "generic/base_cleanable.hpp"
#include "generic/base_serializable.hpp"
#include "generic/base_updatable.hpp"
#include "model/mesh_triangle.hpp"
#include "model/molecule.hpp"
#include "model/path.hpp"
#include <vector>

namespace VTX
{
	namespace Object3D
	{
		class Scene : public Generic::BaseUpdatable, public Generic::BaseCleanable, public Generic::BaseSerializable
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

			inline const Math::AABB & getAABB() const { return _aabb; }

			void addMolecule( MoleculePtr const p_molecule )
			{
				_molecules.emplace( p_molecule, 0.f );
				_aabb.extend( p_molecule->getGlobalPositionsAABB() );
			}
			void removeMolecule( MoleculePtr const p_molecule )
			{
				_molecules.erase( p_molecule );
				_computeAABB();
			}
			void addPath( PathPtr const p_path ) { _paths.emplace_back( p_path ); }
			void addMesh( MeshTrianglePtr const p_mesh )
			{
				_meshes.emplace_back( p_mesh );
				_aabb.extend( p_mesh->getAABB() );
			}
			void removeMesh( MeshTrianglePtr const p_mesh )
			{
				_meshes.erase( std::find( _meshes.begin(), _meshes.end(), p_mesh ) );
				_computeAABB();
			}

			inline Camera &						 getCamera() { return _camera; }
			inline const Camera &				 getCamera() const { return _camera; }
			inline MapMoleculePtrFloat &		 getMolecules() { return _molecules; };
			inline const MapMoleculePtrFloat &	 getMolecules() const { return _molecules; };
			inline const VectorPathPtr &		 getPaths() const { return _paths; };
			inline const VectorMeshTrianglePtr & getMeshes() const { return _meshes; };

			virtual void update( const double ) override;

			virtual void		   fromJson( nlohmann::json & ) override;
			virtual nlohmann::json toJson() const override;

		  private:
			void _computeAABB()
			{
				_aabb.invalidate();
				for ( const PairMoleculePtrFloat mol : _molecules )
				{
					_aabb.extend( mol.first->getGlobalPositionsAABB() );
				}
				for ( const MeshTrianglePtr mesh : _meshes )
				{
					_aabb.extend( mesh->getAABB() );
				}
			}

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
