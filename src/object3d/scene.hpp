#ifndef __VTX_SCENE__
#define __VTX_SCENE__

#ifdef _MSC_VER
#pragma once
#endif

#include "../model/model_molecule.hpp"
#include "camera/camera_orbit.hpp"
#include <vector>

namespace VTX
{
	namespace Object3D
	{
		class Scene
		{
		  public:
			Scene() = default;
			~Scene();

			void addMolecule( Model::ModelMolecule * const p_molecule ) { _molecules.emplace_back( p_molecule ); }
			inline Camera::CameraOrbit &			   getCameraOrbit() { return _camera; }
			inline std::vector<Model::ModelMolecule *> getMolecules() const { return _molecules; };

		  private:
			Camera::CameraOrbit					_camera	   = Camera::CameraOrbit();
			std::vector<Model::ModelMolecule *> _molecules = std::vector<Model::ModelMolecule *>();
		};
	} // namespace Object3D
} // namespace VTX
#endif
