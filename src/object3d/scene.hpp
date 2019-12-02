#ifndef __VTX_SCENE__
#define __VTX_SCENE__

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
			~Scene();
			void addMolecule( Model::ModelMolecule * const p_molecule ) { _molecules.emplace_back( p_molecule ); }

		  private:
			Camera::CameraOrbit					_camera	   = Camera::CameraOrbit();
			std::vector<Model::ModelMolecule *> _molecules = std::vector<Model::ModelMolecule *>();
		};
	} // namespace Object3D
} // namespace VTX
#endif
