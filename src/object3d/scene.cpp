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
			Generic::clearMapAsKey( _molecules );
			Generic::clearVector( _meshes );
			Generic::clearVector( _paths );
		}

		void Scene::update( const double p_deltaTime )
		{
			// TOCHECK: do that in state or in scene?
			// (let that here instead of doing the exact same things in all states for the moment)

			// Dynamic.
			for ( PairMoleculePtrFloat & pair : _molecules )
			{
				MoleculePtr const molecule = pair.first;
				float			  time	   = pair.second;

				uint frameCount = molecule->getFrameCount();
				if ( molecule->isPlaying() == false || frameCount < 2 ) { continue; }

				uint frame = molecule->getFrame();
				uint fps   = molecule->getFPS();

				uint nextFrame = frame;

				if ( fps == 0u ) { molecule->setFrame( ++nextFrame % frameCount ); }
				else
				{
					time += float( p_deltaTime );
					float offset = 1.f / float( fps );
					while ( time >= offset )
					{
						nextFrame++;
						time -= offset;
					}

					pair.second = time;
					if ( nextFrame != frame ) { molecule->setFrame( nextFrame % frameCount ); }
				}
			}

			// Auto rotate.
			if ( Setting::Controller::autoRotateSpeed.x != 0.f || Setting::Controller::autoRotateSpeed.y != 0.f
				 || Setting::Controller::autoRotateSpeed.z != 0.f )
			{
				for ( const PairMoleculePtrFloat & pair : _molecules )
				{
					pair.first->rotate( float( p_deltaTime ) * Setting::Controller::autoRotateSpeed.x, VEC3F_X );
					pair.first->rotate( float( p_deltaTime ) * Setting::Controller::autoRotateSpeed.y, VEC3F_Y );
					pair.first->rotate( float( p_deltaTime ) * Setting::Controller::autoRotateSpeed.z, VEC3F_Z );
				}

				for ( const MeshTrianglePtr & mesh : _meshes )
				{
					mesh->rotate( float( p_deltaTime ) * Setting::Controller::autoRotateSpeed.x, VEC3F_X );
					mesh->rotate( float( p_deltaTime ) * Setting::Controller::autoRotateSpeed.y, VEC3F_Y );
					mesh->rotate( float( p_deltaTime ) * Setting::Controller::autoRotateSpeed.z, VEC3F_Z );
				}
			}
		} // namespace Object3D

	} // namespace Object3D
} // namespace VTX
