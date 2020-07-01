#include "scene.hpp"
#include "generic/factory.hpp"
#include "math/transform.hpp"
#include "vtx_app.hpp"

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
				if ( molecule->isPlaying() == false || frameCount < 2 )
				{
					continue;
				}

				uint frame = molecule->getFrame();
				uint fps   = molecule->getFPS();

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

		void Scene::fromJson( nlohmann::json & p_json )
		{
			// Just clean paths for the moment.
			clean();

			for ( nlohmann::json & jsonMolecule : p_json[ "MOLECULES" ] )
			{
				Model::Path * path = Generic::create<Model::Path>();
				addPath( path );
				path->fromJson( jsonMolecule );
			}

			for ( nlohmann::json & jsonPath : p_json[ "PATHS" ] )
			{
				Model::Path * path = Generic::create<Model::Path>();
				addPath( path );
				path->fromJson( jsonPath );
			}
		}

		nlohmann::json Scene::toJson() const
		{
			nlohmann::json jsonArrayMolecules = nlohmann::json::array();
			for ( const PairMoleculePtrFloat & pair : _molecules )
			{
				jsonArrayMolecules.emplace_back( pair.first->toJson() );
			}

			nlohmann::json jsonArrayPaths = nlohmann::json::array();
			for ( const Model::Path * const path : _paths )
			{
				jsonArrayPaths.emplace_back( path->toJson() );
			}

			return { { "MOLECULES", jsonArrayMolecules }, { "PATHS", jsonArrayPaths } };
		}

	} // namespace Object3D
} // namespace VTX
