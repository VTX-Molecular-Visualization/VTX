#include "scene.hpp"
#include "action/select.hpp"
#include "generic/factory.hpp"
#include "math/transform.hpp"
#include "setting.hpp"

namespace VTX
{
	namespace Object3D
	{
		Scene::Scene()
		{
			Model::Path * path = Generic::create<Model::Path>();
			addPath( path );
			VTXApp::get().getActionManager().execute( new Action::Select( *path ) );
		}

		void Scene::clean()
		{
			Generic::clearVector( _molecules );
			Generic::clearVector( _paths );
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
