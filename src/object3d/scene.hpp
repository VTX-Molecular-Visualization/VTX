#ifndef __VTX_SCENE__
#define __VTX_SCENE__

#ifdef _MSC_VER
#pragma once
#endif

#include "../generic/base_updatable.hpp"
#include "../model/model_checkpoint.hpp"
#include "../model/model_molecule.hpp"
#include "camera.hpp"
#include <vector>

namespace VTX
{
	namespace Object3D
	{
		class Scene : public Generic::BaseUpdatable
		{
		  public:
			using MoleculePtr			 = Model::ModelMolecule *;
			using CheckpointPtr			 = Model::ModelCheckpoint *;
			using VectorModelMoleculePtr = std::vector<MoleculePtr>;
			using VectorCheckpointPtr	 = std::vector<CheckpointPtr>;

			Scene() = default;
			~Scene();

			void clear();

			void addMolecule( MoleculePtr const p_molecule ) { _molecules.emplace_back( p_molecule ); }
			void addCheckpoint( CheckpointPtr const p_checkpoint ) { _checkpoints.emplace_back( p_checkpoint ); }

			inline Camera &					  getCamera() { return _camera; }
			inline std::vector<MoleculePtr>	  getMolecules() const { return _molecules; };
			inline std::vector<CheckpointPtr> getCheckpoints() const { return _checkpoints; };

			virtual void update( const double ) override;

		  private:
			Camera				   _camera		= Camera();
			VectorModelMoleculePtr _molecules	= VectorModelMoleculePtr();
			VectorCheckpointPtr	   _checkpoints = VectorCheckpointPtr();
		};
	} // namespace Object3D
} // namespace VTX
#endif
