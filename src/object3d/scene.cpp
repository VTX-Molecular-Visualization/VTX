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
			// (let that here instead of doing the exact same things in states for the moment)

			// Dynamic.
			for ( MoleculePtr const molecule : _molecules )
			{
				if ( molecule->isPlaying() == false ) { break; }
				uint currentFrame = molecule->getCurrentFrame();
				if ( molecule->getFPS() == 0 ) { molecule->setFrame( ( ++currentFrame ) % molecule->getFrameCount() ); }
				else
				{
					uint nextFrame = molecule->getNextFrame( float( p_deltaTime ) );
					if ( nextFrame != currentFrame ) { molecule->setFrame( nextFrame ); }
				}
			}

			// Auto rotate.
			if ( Setting::Controller::autoRotateSpeed.x != 0.f || Setting::Controller::autoRotateSpeed.y != 0.f
				 || Setting::Controller::autoRotateSpeed.z != 0.f )
			{
				for ( MoleculePtr const molecule : _molecules )
				{
					molecule->rotate( float( p_deltaTime ) * Setting::Controller::autoRotateSpeed.x, VEC3F_X );
					molecule->rotate( float( p_deltaTime ) * Setting::Controller::autoRotateSpeed.y, VEC3F_Y );
					molecule->rotate( float( p_deltaTime ) * Setting::Controller::autoRotateSpeed.z, VEC3F_Z );
				}
			}
		}

	} // namespace Object3D
} // namespace VTX
