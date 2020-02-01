#ifndef __VTX_SCENE__
#define __VTX_SCENE__

#ifdef _MSC_VER
#pragma once
#endif

#include "../generic/base_updatable.hpp"
#include "../model/model_molecule.hpp"
#include "../model/model_path.hpp"
#include "../util/type.hpp"
#include "camera.hpp"
#include <vector>

namespace VTX
{
	namespace Object3D
	{
		class Scene : public Generic::BaseUpdatable
		{
		  public:
			using MoleculePtr		= Model::ModelMolecule *;
			using PathPtr			= Model::ModelPath *;
			using VectorMoleculePtr = std::vector<MoleculePtr>;
			using VectorPathPtr		= std::vector<PathPtr>;

			Scene();
			~Scene();

			void clear();

			void addMolecule( MoleculePtr const p_molecule ) { _molecules.emplace_back( p_molecule ); }
			void addPath( PathPtr const p_path ) { _paths.emplace_back( p_path ); }

			inline Camera &			 getCamera() { return _camera; }
			inline VectorMoleculePtr getMolecules() const { return _molecules; };
			inline VectorPathPtr	 getPaths() const { return _paths; };

			virtual void update( const double ) override;

		  private:
			Camera			  _camera	 = Camera();
			VectorMoleculePtr _molecules = VectorMoleculePtr();
			VectorPathPtr	  _paths	 = VectorPathPtr();
		};
	} // namespace Object3D
} // namespace VTX
#endif
