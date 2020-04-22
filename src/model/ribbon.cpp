#include "ribbon.hpp"

namespace VTX
{
	namespace Model
	{
		Ribbon::Ribbon( const Molecule & p_molecule )
		{
			VTX_DEBUG( "Building secondary structure..." );

			const Molecule::AtomPositionsFrame & positions = p_molecule.getAtomPositionFrame( 0 );

			uint vIndex = 0;

			// Loop over chains.
			for ( uint chainIdx = 0; chainIdx < p_molecule.getChainCount(); ++chainIdx )
			{
				Vec3f		  flipTestV	   = VEC3F_ZERO;
				Math::BSpline splineCenter = Math::BSpline();
				Math::BSpline splineSide1  = Math::BSpline();
				Math::BSpline splineSide2  = Math::BSpline();

				VTX_DEBUG( "Building secondary structure... chain " + std::to_string( chainIdx ) );
				const Chain & chain			  = p_molecule.getChain( chainIdx );
				const Vec3f & chainColor	  = chain.getColor();
				uint		  residueCount	  = chain.getResidueCount();
				uint		  idxFirstResidue = chain.getIdFirstResidue();

				// Loop over residues
				uint residueGlobalIdx = 0;
				for ( uint residueIdx = 0; residueIdx < residueCount; ++residueIdx, ++residueGlobalIdx )
				{
					VTX_DEBUG( "Building secondary structure... residue " + std::to_string( residueIdx ) );
					const Residue &					   residue = p_molecule.getResidue( idxFirstResidue + residueIdx );
					const Residue::SECONDARY_STRUCTURE ss	   = residue.getSecondaryStructure();

					// First residue
					if ( residueIdx == 0 )
					{
						const Residue & residue2 = p_molecule.getResidue( idxFirstResidue + 1 );

						const Model::Atom * CA1 = residue.findFirstAtomByName( "CA" );
						const Model::Atom * OX1 = residue.findFirstAtomByName( "O" );
						const Model::Atom * CA2 = residue2.findFirstAtomByName( "CA" );

						if ( CA1 == nullptr || OX1 == nullptr || CA2 == nullptr )
						{
							VTX_DEBUG( "Failed to get atoms for peptide plane" );
							continue;
						}

						const Vec3f & positionCA1 = positions[ CA1->getIndex() ];
						const Vec3f & positionOX1 = positions[ OX1->getIndex() ];
						const Vec3f & positionCA2 = positions[ CA2->getIndex() ];

						_addControlPoints( positionCA1,
										   positionOX1,
										   positionCA2,
										   residue.getSecondaryStructure(),
										   flipTestV,
										   splineCenter,
										   splineCenter,
										   splineSide2 );

						splineSide1.copyPoint( 3, 2 );
						splineCenter.copyPoint( 3, 2 );
						splineSide2.copyPoint( 3, 2 );

						const Residue & residue3 = p_molecule.getResidue( idxFirstResidue + 2 );

						const Model::Atom * OX2 = residue2.findFirstAtomByName( "O" );
						const Model::Atom * CA3 = residue3.findFirstAtomByName( "CA" );

						if ( CA2 == nullptr || OX2 == nullptr || CA3 == nullptr )
						{
							VTX_DEBUG( "Failed to get atoms for peptide plane" );
							continue;
						}

						const Vec3f & positionOX2 = positions[ OX2->getIndex() ];
						const Vec3f & positionCA3 = positions[ CA3->getIndex() ];

						_addControlPoints( positionCA2,
										   positionOX2,
										   positionCA3,
										   residue.getSecondaryStructure(),
										   flipTestV,
										   splineCenter,
										   splineSide1,
										   splineSide2 );

						Vec3f p2 = splineCenter.getPoint( 2u );
						Vec3f p3 = splineCenter.getPoint( 3u );
						Vec3f p1 = Util::Math::linearComb( 2.f, p2, -1.f, p3 );

						splineCenter.setPoint( 1, p1 );
						splineSide1.setPoint( 1,
											  Util::Math::linearComb( 1.f, p1, RIBBON_WIDTH[ int( ss ) ], flipTestV ) );
						splineSide2.setPoint(
							1, Util::Math::linearComb( 1.f, p1, -RIBBON_WIDTH[ int( ss ) ], flipTestV ) );

						Vec3f p0 = Util::Math::linearComb( 2.f, p1, -1.f, p2 );

						splineCenter.setPoint( 0, p0 );
						splineSide1.setPoint( 0,
											  Util::Math::linearComb( 1.f, p0, RIBBON_WIDTH[ int( ss ) ], flipTestV ) );
						splineSide2.setPoint(
							0, Util::Math::linearComb( 1.f, p0, -RIBBON_WIDTH[ int( ss ) ], flipTestV ) );
					}
					else
					{
						splineCenter.shiftPoints();
						splineSide1.shiftPoints();
						splineSide2.shiftPoints();

						if ( ( residueIdx == residueCount - 1 ) || ( residueIdx == residueCount - 2 ) )
						{
							Vec3f p1 = splineCenter.getPoint( 1u );
							Vec3f p2 = splineCenter.getPoint( 2u );

							Vec3f p3 = Util::Math::linearComb( 2.f, p2, -1.f, p1 );

							splineCenter.setPoint( 3, p3 );
							splineSide1.setPoint(
								3, Util::Math::linearComb( 1.f, p3, RIBBON_WIDTH[ int( ss ) ], flipTestV ) );
							splineSide2.setPoint(
								3, Util::Math::linearComb( 1.f, p3, -RIBBON_WIDTH[ int( ss ) ], flipTestV ) );
						}
						else
						{
							const Residue & residue2 = p_molecule.getResidue( idxFirstResidue + residueIdx + 1 );
							const Residue & residue3 = p_molecule.getResidue( idxFirstResidue + residueIdx + 2 );

							const Model::Atom * CA2 = residue2.findFirstAtomByName( "CA" );
							const Model::Atom * OX2 = residue2.findFirstAtomByName( "O" );
							const Model::Atom * CA3 = residue3.findFirstAtomByName( "CA" );

							if ( CA2 == nullptr || OX2 == nullptr || CA3 == nullptr )
							{
								VTX_DEBUG( "Failed to get atoms for peptide plane" );
								continue;
							}

							const Vec3f & positionCA2 = positions[ CA2->getIndex() ];
							const Vec3f & positionOX2 = positions[ OX2->getIndex() ];
							const Vec3f & positionCA3 = positions[ CA3->getIndex() ];

							_addControlPoints( positionCA2,
											   positionOX2,
											   positionCA3,
											   residue.getSecondaryStructure(),
											   flipTestV,
											   splineCenter,
											   splineSide1,
											   splineSide2 );
						}
					}

					_computeTriangleMesh( splineCenter, splineSide1, splineSide2, chainColor, vIndex );
				}
			}
		}

		void Ribbon::_addControlPoints( const Vec3f &					   p_CA0Pos,
										const Vec3f &					   p_OX0Pos,
										const Vec3f &					   p_CA1Pos,
										const Residue::SECONDARY_STRUCTURE p_ss,
										Vec3f &							   po_flipTestV,
										Math::BSpline &					   po_splineCenter,
										Math::BSpline &					   po_splineSide1,
										Math::BSpline &					   po_splineSide2 )
		{
			Vec3f A, B, C, D, p0, cpt0, cpt1, cpt2;

			A = p_CA1Pos - p_CA0Pos;
			B = p_OX0Pos - p_CA0Pos;

			// Vector normal to the peptide plane (pointing outside in the case of the
			// alpha helix).
			C = Util::Math::cross( A, B );

			// Vector contained in the peptide plane (perpendicular to its direction).
			D = Util::Math::cross( C, A );

			Util::Math::normalizeSelf( C );
			Util::Math::normalizeSelf( D );

			// Flipping test (to avoid self crossing in the strands).
			if ( ( p_ss != Residue::SECONDARY_STRUCTURE::HELIX )
				 && ( Util::Math::radians( 90.f ) < Util::Math::angle( po_flipTestV, D ) ) )
			{
				D = D * -1.f; // flip detected, the plane vector is inverted
			}

			// The central control point is constructed
			cpt0 = Util::Math::linearComb( 0.5f, p_CA0Pos, 0.5f, p_CA1Pos );
			po_splineCenter.setPoint( 3, cpt0 );

			if ( p_ss == Residue::SECONDARY_STRUCTURE::HELIX )
			{
				// When residue i is contained in a helix, the control point is moved away
				// from the helix axis, along the C direction.
				p0	 = po_splineCenter.getPoint( 3u );
				cpt0 = Util::Math::linearComb( 1.f, p0, /*handedness **/ HELIX_DIAM, C );
				po_splineCenter.setPoint( 3, cpt0 );
			}

			// The control points for the side ribbons are constructed.
			cpt1 = Util::Math::linearComb( 1.f, cpt0, RIBBON_WIDTH[ int( p_ss ) ], D );
			po_splineSide1.setPoint( 3, cpt1 );

			cpt2 = Util::Math::linearComb( 1.f, cpt0, -RIBBON_WIDTH[ int( p_ss ) ], D );
			po_splineSide2.setPoint( 3, cpt2 );

			// Saving the plane vector (for the flipping test in the next call)
			po_flipTestV = D;
		}

		void Ribbon::_computeTriangleMesh( const Math::BSpline & p_splineCenter,
										   const Math::BSpline & p_splineSide1,
										   const Math::BSpline & p_splineSide2,
										   const Vec3f &		 p_color,
										   uint &				 po_vIndex )
		{
			Vec3f pointCenter0, pointCenter1;
			Vec3f pointSide10, pointSide11, pointSide20, pointSide21;
			Vec3f transversal, tangent, normal0, normal1;
			Vec3f CP0, CP1, S1P0, S1P1, S2P0, S2P1, norm0, norm1;
			float t = 0.f;

			// The initial geometry is generated
			pointCenter1 = p_splineCenter.getPoint( 0.f );
			pointSide11	 = p_splineSide1.getPoint( 0.f );
			pointSide21	 = p_splineSide2.getPoint( 0.f );

			// The tangents at the three previous points are the same
			tangent = p_splineSide2.getTangent( 0.f );

			// Vector transversal to the ribbon
			transversal = pointSide11 - pointSide21;
			normal1		= Util::Math::cross( transversal, tangent );
			Util::Math::normalizeSelf( normal1 );

			Vec3f leftNormal0, leftNormal1, rightNormal0, rightNormal1;
			for ( uint step = 1; step <= DETAIL_LEVEL; ++step )
			{
				t = ( 1.f / (float)DETAIL_LEVEL ) * (float)step;

				// The geometry of the previous iteration is saved
				pointSide10	 = pointSide11;
				pointCenter0 = pointCenter1;
				pointSide20	 = pointSide21;
				normal0		 = normal1;

				// The new geometry is generated
				pointSide11	 = p_splineSide1.getPoint( t );
				pointCenter1 = p_splineCenter.getPoint( t );
				pointSide21	 = p_splineSide2.getPoint( t );

				// The tangents at the three previous points are the same
				tangent = p_splineSide2.getTangent( t );

				// Vector transversal to the ribbon
				transversal = pointSide11 - pointSide21;
				normal1		= Util::Math::cross( transversal, tangent );
				Util::Math::normalizeSelf( normal1 );

				// The x coordinates must be flipped, but the original vectors can't be
				// modified because they will be reused in the next iteration.
				S1P0 = Vec3f( -pointSide10.x, pointSide10.y, pointSide10.z );
				S1P1 = Vec3f( -pointSide11.x, pointSide11.y, pointSide11.z );

				S2P0 = Vec3f( -pointSide20.x, pointSide20.y, pointSide20.z );
				S2P1 = Vec3f( -pointSide21.x, pointSide21.y, pointSide21.z );

				CP0 = Vec3f( -pointCenter0.x, pointCenter0.y, pointCenter0.z );
				CP1 = Vec3f( -pointCenter1.x, pointCenter1.y, pointCenter1.z );

				norm0 = Vec3f( -normal0.x, normal0.y, normal0.z );
				norm1 = Vec3f( -normal1.x, normal1.y, normal1.z );

				// Left and right may be reversed, but either way,
				// these normals point outwards from the ribbons, horizontally.
				leftNormal0 = Util::Math::normalize( S1P0 - CP0 );
				leftNormal1 = Util::Math::normalize( S1P1 - CP1 );

				rightNormal0 = Util::Math::normalize( S2P0 - CP0 );
				rightNormal1 = Util::Math::normalize( S2P1 - CP1 );

				// The (Sid1Point0, Sid1Point1, CentPoint1) triangle is added.
				_vertices.push_back( S1P0 );
				_normals.push_back( -norm0 );

				_vertices.push_back( S1P1 );
				_normals.push_back( -norm1 );

				_vertices.push_back( CP1 );
				_normals.push_back( -norm1 );

				// and duplicated above
				_vertices.push_back( S1P0 + THICKNESS * norm0 );
				_normals.push_back( norm0 );

				_vertices.push_back( S1P1 + THICKNESS * norm1 );
				_normals.push_back( norm1 );

				_vertices.push_back( CP1 + THICKNESS * norm1 );
				_normals.push_back( norm1 );

				// The (Sid1Point0, CentPoint1, CentPoint0) triangle is added.
				_vertices.push_back( S1P0 );
				_normals.push_back( -norm0 );

				_vertices.push_back( CP1 );
				_normals.push_back( -norm1 );

				_vertices.push_back( CP0 );
				_normals.push_back( -norm0 );

				// and duplicated above
				_vertices.push_back( S1P0 + THICKNESS * norm0 );
				_normals.push_back( norm0 );

				_vertices.push_back( CP1 + THICKNESS * norm1 );
				_normals.push_back( norm1 );

				_vertices.push_back( CP0 + THICKNESS * norm0 );
				_normals.push_back( norm0 );

				// (Sid2Point0, Sid2Point1, CentPoint1) triangle is added.
				_vertices.push_back( S2P0 );
				_normals.push_back( -norm0 );

				_vertices.push_back( S2P1 );
				_normals.push_back( -norm1 );

				_vertices.push_back( CP1 );
				_normals.push_back( -norm1 );

				// and duplicated above
				_vertices.push_back( S2P0 + THICKNESS * norm0 );
				_normals.push_back( norm0 );

				_vertices.push_back( S2P1 + THICKNESS * norm1 );
				_normals.push_back( norm1 );

				_vertices.push_back( CP1 + THICKNESS * norm1 );
				_normals.push_back( norm1 );

				// (Sid2Point0, CentPoint1, CentPoint0) triangle is added.
				_vertices.push_back( S2P0 );
				_normals.push_back( -norm0 );

				_vertices.push_back( CP1 );
				_normals.push_back( -norm1 );

				_vertices.push_back( CP0 );
				_normals.push_back( -norm0 );

				// and duplicated above
				_vertices.push_back( S2P0 + THICKNESS * norm0 );
				_normals.push_back( norm0 );

				_vertices.push_back( CP1 + THICKNESS * norm1 );
				_normals.push_back( norm1 );

				_vertices.push_back( CP0 + THICKNESS * norm0 );
				_normals.push_back( norm0 );

				// Duplicating the side vertices and giving them the proper normals
				// for the sides of the thick ribbons
				_vertices.push_back( S1P0 );
				_normals.push_back( leftNormal0 );

				_vertices.push_back( S1P1 );
				_normals.push_back( leftNormal1 );

				_vertices.push_back( S2P0 );
				_normals.push_back( rightNormal0 );

				_vertices.push_back( S2P1 );
				_normals.push_back( rightNormal1 );

				// and duplicating them again, this time raising them as well
				_vertices.push_back( S1P0 + THICKNESS * norm0 );
				_normals.push_back( leftNormal0 );

				_vertices.push_back( S1P1 + THICKNESS * norm1 );
				_normals.push_back( leftNormal1 );

				_vertices.push_back( S2P0 + THICKNESS * norm0 );
				_normals.push_back( rightNormal0 );

				_vertices.push_back( S2P1 + THICKNESS * norm1 );
				_normals.push_back( rightNormal1 );

				// Colors.
				for ( uint t = 0; t < 32; ++t )
				{
					_colors.push_back( p_color );
				}

				// Triangle indices.
				for ( uint t = 0; t < 24; ++t )
				{
					_indices.push_back( po_vIndex + t );
				}

				_indices.push_back( po_vIndex + 24 );
				_indices.push_back( po_vIndex + 25 );
				_indices.push_back( po_vIndex + 28 );

				_indices.push_back( po_vIndex + 25 );
				_indices.push_back( po_vIndex + 28 );
				_indices.push_back( po_vIndex + 29 );

				_indices.push_back( po_vIndex + 26 );
				_indices.push_back( po_vIndex + 27 );
				_indices.push_back( po_vIndex + 30 );

				_indices.push_back( po_vIndex + 27 );
				_indices.push_back( po_vIndex + 30 );
				_indices.push_back( po_vIndex + 31 );

				po_vIndex += 32;
			}
		}

	} // namespace Model
} // namespace VTX
