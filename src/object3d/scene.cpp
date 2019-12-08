#include "scene.hpp"

namespace VTX
{
	namespace Object3D
	{
		Scene::~Scene()
		{
			for ( Model::ModelMolecule * molecule : _molecules )
			{
				delete molecule;
			}

			_molecules.clear();
		}

	} // namespace Object3D
} // namespace VTX
