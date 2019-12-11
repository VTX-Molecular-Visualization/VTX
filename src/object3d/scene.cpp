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

		void Scene::setupMoleculeViews() const
		{
			for ( Model::ModelMolecule * molecule : _molecules )
			{
				molecule->setup3DViews();
			}
		}

	} // namespace Object3D
} // namespace VTX
