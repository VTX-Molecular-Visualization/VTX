#include "rt_scene_optix.hpp"

namespace VTX
{
	namespace Renderer
	{
		namespace Optix
		{
			Scene::~Scene() { _spheresDevBuffer.free(); }

			void Scene::add( const Model::Molecule * p_molecule, const Generic::REPRESENTATION p_representation )
			{
				// TODO: only handle spheres and the first frame for the moment...
				switch ( p_representation )
				{
				case Generic::REPRESENTATION::VAN_DER_WAALS:
				case Generic::REPRESENTATION::SAS: _addSpheres( p_molecule, p_representation ); break;
				case Generic::REPRESENTATION::BALL_AND_STICK:
				case Generic::REPRESENTATION::STICK:
				case Generic::REPRESENTATION::CARTOON:
				default: VTX_WARNING( "Not implemented" ); break;
				}
			}

			void Scene::_addSpheres( const Model::Molecule *	   p_molecule,
									 const Generic::REPRESENTATION p_representation )
			{
				const float radiusAdd = p_representation == Generic::REPRESENTATION::SAS ? 1.4f : 0.f;
				const uint	nbAtoms	  = p_molecule->getAtomCount();
				std::cout << "Adding " << nbAtoms << "(size on GPU: " << nbAtoms * sizeof( Optix::Sphere ) << ")"
						  << std::endl;

				const std::vector<Vec3f> & atomPositions = p_molecule->getAtomPositionFrame( 0 );

				_spheres.resize( nbAtoms );

				std::vector<float3> colors;

				for ( uint i = 0; i < nbAtoms; ++i )
				{
					const Vec3f &	   p  = atomPositions[ i ];
					const float		   r  = p_molecule->getAtomRadius( i ) + radiusAdd;
					const Color::Rgb & c  = p_molecule->getAtomColor( i );
					_spheres[ i ]._center = { p.x, p.y, p.z };
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
