#include "rt_scene_optix.hpp"

namespace VTX
{
	namespace Renderer
	{
		namespace Optix
		{
			Scene::~Scene() { _spheresDevBuffer.free(); }

			void Scene::add( const Model::Molecule * p_molecule )
			{
				// TODO: only handle spheres...
				const uint nbAtoms = p_molecule->getAtomCount();
				std::cout << "Adding " << nbAtoms << "(size on GPU: " << nbAtoms * sizeof( Optix::Sphere ) << ")"
						  << std::endl;

				const std::vector<Vec3f> & atomPositions = p_molecule->getAtomPositionFrame( 0 );

				_spheres.resize( nbAtoms );

				std::vector<float3> colors;

				for ( uint i = 0; i < nbAtoms; ++i )
				{
					const Vec3f &	   p  = atomPositions[ i ];
					const float		   r  = p_molecule->getAtomRadius( i );
					const Color::Rgb & c  = p_molecule->getAtomColor( i );
					_spheres[ i ]._center = make_float3( p.x, p.y, p.z );
					_spheres[ i ]._radius = r;
					uint colorId		  = INVALID_ID;
					for ( uint j = 0; j < uint( colors.size() ); ++j )
					{
						if ( colors[ j ].x == c.getR() && colors[ j ].y == c.getG() && colors[ j ].z == c.getB() )
						{
							colorId = j;
							break;
						}
					}
					if ( colorId == INVALID_ID )
					{
						colorId = uint( colors.size() );
						colors.emplace_back( make_float3( c.getR(), c.getG(), c.getB() ) );
					}
					_spheres[ i ]._colorId = colorId;
				}
				_spheresDevBuffer.malloc( _spheres.size() * sizeof( Optix::Sphere ) );
				_spheresDevBuffer.memcpyHostToDevice( _spheres.data(), _spheres.size() );

				_colorsDevBuffer.malloc( colors.size() * sizeof( float3 ) );
				_colorsDevBuffer.memcpyHostToDevice( colors.data(), colors.size() );
			}

		} // namespace Optix
	}	  // namespace Renderer
} // namespace VTX
