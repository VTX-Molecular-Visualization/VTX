#ifndef __RIBBON__
#define __RIBBON__

#include "define.hpp"
#include "model/chain.hpp"
#include "model/mesh_triangle.hpp"
#include "model/residue.hpp"
#include "peptide_plane.hpp"
#include "util/math.hpp"

namespace VTX
{
	namespace Cartoon
	{
		// Move in defines or other constants file.
		const int splineSteps	= 32 / 2;
		const int profileDetail = 16 / 2;

		const float ribbonWidth	   = 2.0f;
		const float ribbonHeight   = 0.125f;
		const float ribbonOffset   = 1.5f;
		const float arrowHeadWidth = 3.0f;
		const float arrowWidth	   = 2.0f;
		const float arrowHeight	   = 0.5f;
		const float tubeSize	   = 0.25f;

		void createChainMesh( const Model::Chain & p_chain )
		{
			const Model::Molecule &			   molecule		= *p_chain.getMoleculePtr();
			std::vector<Model::MeshTriangle>   chainMeshes	= std::vector<Model::MeshTriangle>();
			std::vector<Cartoon::PeptidePlane> planes		= std::vector<Cartoon::PeptidePlane>();
			int								   residueCount = p_chain.getResidueCount();

			// TODO: c'est de la merde.
			for ( int i = -1; i < residueCount; ++i )
			{
				int id	= i;
				int id1 = i + 1;
				int id2 = i + 2;

				if ( i == -1 )
				{
					id	= 0;
					id1 = 0;
					id2 = i + 1;
				}
				else if ( i == residueCount - 2 )
				{
					id	= i;
					id1 = i + 1;
					id2 = i + 1;
				}
				else if ( i == residueCount - 1 )
				{
					id	= i - 1;
					id1 = i;
					id2 = i;
				}

				const Model::Residue & r1 = molecule.getResidue( id );
				const Model::Residue & r2 = molecule.getResidue( id1 );
				const Model::Residue & r3 = molecule.getResidue( id2 );

				PeptidePlane plane = PeptidePlane( r1, r2, r3 );

				// Missing atom to compute plane.
				if ( plane.isValid() == false ) { continue; }
				// Make sure to start at the first CA position.
				if ( i <= 0 )
				{
					const Model::Atom * const CA = r1.findFirstAtomByName( "CA" );
					plane.setPostion( molecule.getAtomPositionFrame( 0 )[ CA->getIndex() ] );
				}
				// Make sure to end at the last CA position.
				if ( i >= residueCount - 2 )
				{
					const Model::Atom * const CA = r3.findFirstAtomByName( "CA" );
					plane.setPostion( molecule.getAtomPositionFrame( 0 )[ CA->getIndex() ] );
				}

				planes.emplace_back( plane );
			}

			// Hummmm... c pour fère tourné lol?
			Vec3f previousPlane = VEC3D_ZERO;
			for ( uint i = 0; i < planes.size(); i++ )
			{
				PeptidePlane & p = planes[ i ];
				if ( i > 0 && Util::Math::dot( p.getSide(), previousPlane ) < 0.0f ) { p.flip(); }
				previousPlane = p.getSide();
			}

			// ??
			// uint n = planes.size() - 3;
		}
	} // namespace Cartoon
} // namespace VTX

#endif
