#include "rt_scene_optix.hpp"
#include "setting.hpp"

namespace VTX::Renderer::Optix
{
	Scene::~Scene()
	{
		_spheresDevBuffer.free();
		_cylindersDevBuffer.free();
	}

	void Scene::add( const Model::Molecule * p_molecule, const Generic::REPRESENTATION p_representation )
	{
		// TODO: only handle spheres and the first frame for the moment...
		switch ( p_representation )
		{
		case Generic::REPRESENTATION::VAN_DER_WAALS:
		case Generic::REPRESENTATION::SAS: _addSpheres( p_molecule, p_representation ); break;
		case Generic::REPRESENTATION::BALL_AND_STICK:
		case Generic::REPRESENTATION::STICK:
			_addSpheres( p_molecule, p_representation );
			_addCylinders( p_molecule, p_representation );
			break;
		case Generic::REPRESENTATION::CARTOON:
		default: VTX_WARNING( "Not implemented" ); break;
		}
	}

	void Scene::_addSpheres( const Model::Molecule * p_molecule, const Generic::REPRESENTATION p_representation )
	{
		const float radiusAdd = p_representation == Generic::REPRESENTATION::SAS ? 1.4f : 0.f;
		const uint	nbAtoms	  = p_molecule->getAtomCount();
		std::cout << "Adding " << nbAtoms << " spheres (size on GPU: " << nbAtoms * sizeof( Sphere ) << ")"
				  << std::endl;

		const std::vector<Vec3f> & atomPositions = p_molecule->getAtomPositionFrame( 0 );

		const uint offset = uint( _spheres.size() );
		_spheres.resize( offset + nbAtoms );

		for ( uint i = 0; i < nbAtoms; ++i )
		{
			const Vec3f & p = atomPositions[ i ];
			const float	  r = ( p_representation == Generic::REPRESENTATION::BALL_AND_STICK
								|| p_representation == Generic::REPRESENTATION::STICK )
								? Setting::MoleculeView::atomsRadiusFixed
								: p_molecule->getAtomRadius( i ) + radiusAdd;
			const Color::Rgb & c		   = p_molecule->getAtomColor( i );
			_spheres[ i + offset ]._center = { p.x, p.y, p.z };
			_spheres[ i + offset ]._radius = r;
			uint colorId				   = INVALID_ID;
			for ( uint j = 0; j < uint( _colors.size() ); ++j )
			{
				if ( _colors[ j ].x == c.getR() && _colors[ j ].y == c.getG() && _colors[ j ].z == c.getB() )
				{
					colorId = j;
					break;
				}
			}
			if ( colorId == INVALID_ID )
			{
				colorId = uint( _colors.size() );
				_colors.emplace_back( make_float3( c.getR(), c.getG(), c.getB() ) );
			}
			_spheres[ i + offset ]._colorId = colorId;
		}
		_spheresDevBuffer.malloc( _spheres.size() * sizeof( Sphere ) );
		_spheresDevBuffer.memcpyHostToDevice( _spheres.data(), _spheres.size() );

		_colorsDevBuffer.realloc( _colors.size() * sizeof( float3 ) );
		_colorsDevBuffer.memcpyHostToDevice( _colors.data(), _colors.size() );
	}

	void Scene::_addCylinders( const Model::Molecule * p_molecule, const Generic::REPRESENTATION p_representation )
	{
		const uint nbBonds = p_molecule->getBondCount();
		std::cout << "Adding " << nbBonds << " cylinders (size on GPU: " << nbBonds * sizeof( Cylinder ) << ")"
				  << std::endl;

		const std::vector<Vec3f> & atomPositions = p_molecule->getAtomPositionFrame( 0 );

		const uint offset = uint( _cylinders.size() );
		_cylinders.resize( offset + nbBonds );

		for ( uint i = 0; i < nbBonds; ++i )
		{
			const Model::Bond & bond		 = p_molecule->getBond( i );
			const Vec3f &		p0			 = atomPositions[ bond.getIndexFirstAtom() ];
			const Vec3f &		p1			 = atomPositions[ bond.getIndexSecondAtom() ];
			const Color::Rgb &	c			 = p_molecule->getAtomColor( bond.getIndexFirstAtom() );
			_cylinders[ i + offset ]._v0	 = { p0.x, p0.y, p0.z };
			_cylinders[ i + offset ]._v1	 = { p1.x, p1.y, p1.z };
			_cylinders[ i + offset ]._radius = Setting::MoleculeView::bondsRadius;
			uint colorId					 = INVALID_ID;
			for ( uint j = 0; j < uint( _colors.size() ); ++j )
			{
				if ( _colors[ j ].x == c.getR() && _colors[ j ].y == c.getG() && _colors[ j ].z == c.getB() )
				{
					colorId = j;
					break;
				}
			}
			if ( colorId == INVALID_ID )
			{
				colorId = uint( _colors.size() );
				_colors.emplace_back( make_float3( c.getR(), c.getG(), c.getB() ) );
			}
			_cylinders[ i + offset ]._colorId = colorId;
		}
		_cylindersDevBuffer.malloc( _cylinders.size() * sizeof( Cylinder ) );
		_cylindersDevBuffer.memcpyHostToDevice( _cylinders.data(), _cylinders.size() );

		_colorsDevBuffer.realloc( _colors.size() * sizeof( float3 ) );
		_colorsDevBuffer.memcpyHostToDevice( _colors.data(), _colors.size() );
	}
} // namespace VTX::Renderer::Optix
