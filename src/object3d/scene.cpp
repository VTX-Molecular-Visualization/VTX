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

		/*
		std::vector<std::shared_ptr<View::BaseView3DMolecule>> Scene::getMoleculeViewsByRepresentation(
			const View::BaseView3DMolecule::REPRESENTATION p_representation )
		{
			std::vector<std::shared_ptr<View::BaseView3DMolecule>> views
				= std::vector<std::shared_ptr<View::BaseView3DMolecule>>();

			for ( Model::ModelMolecule * const molecule : _molecules )
			{
				views.emplace_back( molecule->createAndGetView3DByRepresentation( p_representation ) );
			}

			return views;
		}
		*/

	} // namespace Object3D
} // namespace VTX
