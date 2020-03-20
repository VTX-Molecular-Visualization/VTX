#include "scene.hpp"
#include "action/transformable_rotate.hpp"
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
		}

		void Scene::clean()
		{
			Generic::clearVector( _molecules );
			Generic::clearVector( _paths );
		}

		void Scene::update( const double p_deltaTime )
		{
			// TOCHECK: do that in state or in scene?
			for ( MoleculePtr const molecule : _molecules )
			{
				VTXApp::get().getActionManager().execute( new Action::TransformableRotate(
					*molecule, (float)p_deltaTime * Setting::Controller::autoRotateSpeed.x, VEC3F_X ) );
				VTXApp::get().getActionManager().execute( new Action::TransformableRotate(
					*molecule, (float)p_deltaTime * Setting::Controller::autoRotateSpeed.y, VEC3F_Y ) );
				VTXApp::get().getActionManager().execute( new Action::TransformableRotate(
					*molecule, (float)p_deltaTime * Setting::Controller::autoRotateSpeed.z, VEC3F_Z ) );
			}
		}

	} // namespace Object3D
} // namespace VTX
