#include "scene.hpp"

namespace VTX
{
	namespace Object3D
	{
		void Scene::addMolecule( const Model::ModelMolecule p_molecule )
		{
			_molecules.emplace_back( p_molecule );
		};

	} // namespace Object3D
} // namespace VTX
