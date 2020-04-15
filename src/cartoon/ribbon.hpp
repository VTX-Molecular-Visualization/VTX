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

		bool diffPP( const int p_id1, const int p_id2 )
		{
			int diff = 1;
			if ( p_id1 < 0 && p_id2 > 0 ) { diff = 2; }
			if ( p_id2 - p_id1 > diff ) { return true; }
			return false;
		}

		bool discontinuity( const PeptidePlane & p_pp1,
							const PeptidePlane & p_pp2,
							const PeptidePlane & p_pp3,
							const PeptidePlane & p_pp4 )
		{
			if ( diffPP( p_pp1.getResidue1().getIndex(), p_pp1.getResidue2().getIndex() )
				 || diffPP( p_pp1.getResidue2().getIndex(), p_pp1.getResidue3().getIndex() ) )
			{ return true; }
			if ( diffPP( p_pp2.getResidue1().getIndex(), p_pp2.getResidue2().getIndex() )
				 || diffPP( p_pp2.getResidue2().getIndex(), p_pp2.getResidue3().getIndex() ) )
			{ return true; }
			if ( diffPP( p_pp3.getResidue1().getIndex(), p_pp3.getResidue2().getIndex() )
				 || diffPP( p_pp3.getResidue2().getIndex(), p_pp3.getResidue3().getIndex() ) )
			{ return true; }
			if ( diffPP( p_pp4.getResidue1().getIndex(), p_pp4.getResidue2().getIndex() )
				 || diffPP( p_pp4.getResidue2().getIndex(), p_pp4.getResidue3().getIndex() ) )
			{ return true; }

			return false;
		}

		Model::MeshTriangle createSegmentMesh( const int			p_curi,
											   const int			p_n,
											   const PeptidePlane & p_pp1,
											   const PeptidePlane & p_pp2,
											   const PeptidePlane & p_pp3,
											   const PeptidePlane & p_pp4 )
		{
			Model::MeshTriangle mesh = Model::MeshTriangle();

			return mesh;
		}

		void createChainMesh( const Model::Chain & p_chain )
		{
			VTX_DEBUG( "createChainMesh" );

			const Model::Molecule &			   molecule		= *p_chain.getMoleculePtr();
			std::vector<Model::MeshTriangle>   chainMeshes	= std::vector<Model::MeshTriangle>();
			std::vector<Cartoon::PeptidePlane> planes		= std::vector<Cartoon::PeptidePlane>();
			int								   residueCount = p_chain.getResidueCount();

			if ( residueCount < 2 ) { return; }

			// Create peptide planes.
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

			// Flip planes.
			Vec3f previousPlane = VEC3D_ZERO;
			for ( uint i = 0; i < planes.size(); i++ )
			{
				PeptidePlane & p = planes[ i ];
				if ( i > 0 && Util::Math::dot( p.getSide(), previousPlane ) < 0.0f ) { p.flip(); }
				previousPlane = p.getSide();
			}

			// ??
			uint n = uint( planes.size() ) - 3u;
			for ( uint i = 0; i < n; i++ )
			{
				PeptidePlane & pp1 = planes[ i ];
				PeptidePlane & pp2 = planes[ i + 1 ];
				PeptidePlane & pp3 = planes[ i + 2 ];
				PeptidePlane & pp4 = planes[ i + 3 ];

				if ( discontinuity( pp1, pp2, pp3, pp4 ) ) { continue; }

				// createSegmentMesh( i, n, pp1, pp2, pp3, pp4 );
			}
		}

	} // namespace Cartoon
} // namespace VTX

#endif
