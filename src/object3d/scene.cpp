#include "scene.hpp"
#include "action/main.hpp"
#include "generic/factory.hpp"
#include "math/transform.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Object3D
	{
		Scene::Scene()
		{
			Model::Path * path = new Model::Path();
			addPath( path );
		}

		void Scene::clear()
		{
			Generic::clearMapAsKey( _molecules );
			Generic::clearVector( _meshes );
			Generic::clearVector( _paths );
		}

		void Scene::update( const double & p_deltaTime )
		{
			// TOCHECK: do that in state or in scene?
			// (let that here instead of doing the exact same things in all states for the moment)

			// Dynamic.
			for ( PairMoleculePtrFloat & pair : _molecules )
			{
				MoleculePtr const molecule = pair.first;
				float			  time	   = pair.second;

				uint frameCount = molecule->getFrameCount();
				if ( molecule->isPlaying() == false || frameCount < 2 )
				{
					continue;
				}

				const uint frame = molecule->getFrame();
				const uint fps	 = molecule->getFPS();

				uint nextFrame = frame;

				if ( fps == 0u )
				{
					molecule->setFrame( ++nextFrame % frameCount );
				}
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
					if ( nextFrame != frame )
					{
						molecule->setFrame( nextFrame % frameCount );
					}
				}
			}

			// Auto rotate.
			if ( VTX_SETTING().autoRotationSpeed != VEC3F_ZERO )
			{
				for ( const PairMoleculePtrFloat & pair : _molecules )
				{
					pair.first->rotate( float( p_deltaTime ) * VTX_SETTING().autoRotationSpeed.x, VEC3F_X );
					pair.first->rotate( float( p_deltaTime ) * VTX_SETTING().autoRotationSpeed.y, VEC3F_Y );
					pair.first->rotate( float( p_deltaTime ) * VTX_SETTING().autoRotationSpeed.z, VEC3F_Z );
				}

				for ( const MeshTrianglePtr & mesh : _meshes )
				{
					mesh->rotate( float( p_deltaTime ) * VTX_SETTING().autoRotationSpeed.x, VEC3F_X );
					mesh->rotate( float( p_deltaTime ) * VTX_SETTING().autoRotationSpeed.y, VEC3F_Y );
					mesh->rotate( float( p_deltaTime ) * VTX_SETTING().autoRotationSpeed.z, VEC3F_Z );
				}
			}
		} // namespace Object3D

	} // namespace Object3D
} // namespace VTX
