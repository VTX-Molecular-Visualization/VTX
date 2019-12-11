#ifndef __VTX_SCENE__
#define __VTX_SCENE__

#ifdef _MSC_VER
#pragma once
#endif

#include "../model/model_molecule.hpp"
#include "camera.hpp"
#include <vector>

namespace VTX
{
	namespace Object3D
	{
		class Scene
		{
		  public:
			using VectorModelMoleculePtr = std::vector<Model::ModelMolecule *>;

			Scene() = default;
			~Scene();

			void addMolecule( Model::ModelMolecule * const p_molecule ) { _molecules.emplace_back( p_molecule ); }
			inline Camera &							   getCamera() { return _camera; }
			inline std::vector<Model::ModelMolecule *> getMolecules() const { return _molecules; };
			void									   setupMoleculeViews() const;

		  private:
			Camera				   _camera	  = Camera();
			VectorModelMoleculePtr _molecules = VectorModelMoleculePtr();
		};
	} // namespace Object3D
} // namespace VTX
#endif
