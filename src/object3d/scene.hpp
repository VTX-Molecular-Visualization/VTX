#ifndef __VTX_SCENE__
#define __VTX_SCENE__

#include "../model/model_molecule.hpp"
#include "../view/base_view_3d_molecule.hpp"
#include "camera/camera_orbit.hpp"
#include <vector>

namespace VTX
{
	namespace Object3D
	{
		// TODO: handle multiple cameras.
		class Scene
		{
		  public:
			Scene()	 = default;
			~Scene() = default;

			void addMolecule( Model::ModelMolecule * const p_molecule ) { _molecules.emplace_back( p_molecule ); }
			inline Camera::CameraOrbit							   getCameraOrbit() const { return _camera; }
			std::vector<std::shared_ptr<View::BaseView3DMolecule>> getMoleculeViewsByRepresentation(
				const View::BaseView3DMolecule::REPRESENTATION );

		  private:
			Camera::CameraOrbit					_camera	   = Camera::CameraOrbit();
			std::vector<Model::ModelMolecule *> _molecules = std::vector<Model::ModelMolecule *>();
		};
	} // namespace Object3D
} // namespace VTX
#endif
