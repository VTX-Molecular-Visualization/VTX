#include "scene.hpp"
#include "../math/transform.hpp"
#include "../settings.hpp"

namespace VTX
{
	namespace Object3D
	{
		Scene::~Scene() { clear(); }

		void Scene::clear()
		{
			for ( Model::ModelMolecule * molecule : _molecules )
			{
				delete molecule;
			}
			_molecules.clear();

			for ( Object3D::Checkpoint * checkpoint : _checkpoints )
			{
				delete checkpoint;
			}
			_checkpoints.clear();
		}

		void Scene::update( const double p_deltaTime )
		{
			for ( Model::ModelMolecule * molecule : _molecules )
			{
				Math::Transform & t = molecule->getTransform();
				t.rotate( (float)p_deltaTime * Setting::Controller::autoRotateSpeed.x, VEC3F_X );
				t.rotate( (float)p_deltaTime * Setting::Controller::autoRotateSpeed.y, VEC3F_Y );
				t.rotate( (float)p_deltaTime * Setting::Controller::autoRotateSpeed.z, VEC3F_Z );
			}
		}

	} // namespace Object3D
} // namespace VTX
