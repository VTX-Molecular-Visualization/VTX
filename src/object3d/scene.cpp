#include "scene.hpp"
#include "math/transform.hpp"
#include "setting.hpp"
#include "util/type.hpp"

namespace VTX
{
	namespace Object3D
	{
		Scene::Scene() {}

		Scene::~Scene() { clear(); }

		void Scene::clear()
		{
			// TOCHECK: Templating auto?
			Util::Type::clearVector<Model::ModelMolecule>( _molecules );
			Util::Type::clearVector( _paths );
		}

		void Scene::update( const double p_deltaTime )
		{
			// TOCHECK: Store BaseTransformable? Object3D super class?
			for ( MoleculePtr molecule : _molecules )
			{
				Math::Transform & t = molecule->getTransform();
				t.rotate( (float)p_deltaTime * Setting::Controller::autoRotateSpeed.x, VEC3F_X );
				t.rotate( (float)p_deltaTime * Setting::Controller::autoRotateSpeed.y, VEC3F_Y );
				t.rotate( (float)p_deltaTime * Setting::Controller::autoRotateSpeed.z, VEC3F_Z );
			}
		}

	} // namespace Object3D
} // namespace VTX
