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
		}

		void Scene::update( const double p_deltaTime )
		{
			for ( Model::ModelMolecule * molecule : _molecules )
			{
				Math::Transform & t = molecule->getTransform();
				t.rotate( p_deltaTime * Setting::Controller::autoRotateSpeedX, VEC3F_X );
				t.rotate( p_deltaTime * Setting::Controller::autoRotateSpeedY, VEC3F_Y );
				t.rotate( p_deltaTime * Setting::Controller::autoRotateSpeedZ, VEC3F_Z );
			}
		}

	} // namespace Object3D
} // namespace VTX
