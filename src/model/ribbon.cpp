#include "ribbon.hpp"
#include "tool/chrono.hpp"

namespace VTX
{
	namespace Model
	{
		Ribbon::Ribbon( const Molecule & p_molecule )
		{
			Tool::Chrono chrono;
			chrono.start();
			VTX_DEBUG( "Creating secondary structure..." );

			const Molecule::AtomPositionsFrame & positions = p_molecule.getAtomPositionFrame( p_molecule.getFrame() );

			uint vIndex = 0;

			// Loop over chains.
			for ( uint chainIdx = 0; chainIdx < p_molecule.getChainCount(); ++chainIdx )
			{
				Vec3f		  flipTestV	   = VEC3F_ZERO;
				Math::BSpline splineCenter = Math::BSpline();
				Math::BSpline splineSide1  = Math::BSpline();
				Math::BSpline splineSide2  = Math::BSpline();

				// VTX_DEBUG( "Building secondary structure... chain " + std::to_string( chainIdx ) );
				const Chain &	   chain		   = p_molecule.getChain( chainIdx );
				const Color::Rgb & chainColor	   = chain.getColor();
				uint			   residueCount	   = chain.getResidueCount();
				uint			   idxFirstResidue = chain.getIdFirstResidue();

				// Not enought residues.
				if ( residueCount < 3 )
				{
					continue;
				}

				// Loop over residues
				uint residueValidCount = 0;
				for ( uint residueIdx = 0; residueIdx < residueCount; ++residueIdx )
				{
					// VTX_DEBUG( "Building secondary structure... residue " + std::to_string( residueIdx ) );
					const Residue &					   residue1 = p_molecule.getResidue( idxFirstResidue + residueIdx );
					const Residue::SECONDARY_STRUCTURE ss		= residue1.getSecondaryStructure();

					// First residue
					if ( residueValidCount == 0 )
					{
						// Avoid crash when not engouth valid residues to start the mesh.
						if ( residueIdx >= residueCount - 2 )
						{
							continue;
						}

						const Residue & residue2 = p_molecule.getResidue( idxFirstResidue + residueIdx + 1 );

						const Model::Atom * CA1 = residue1.findFirstAtomByName( "CA" );
						const Model::Atom * OX1 = residue1.findFirstAtomByName( "O" );
						const Model::Atom * CA2 = residue2.findFirstAtomByName( "CA" );

						if ( CA1 == nullptr || OX1 == nullptr || CA2 == nullptr )
						{
							// VTX_WARNING( "Failed to get atoms for first residue" );
							continue;
						}

						const Vec3f & positionCA1 = positions[ CA1->getIndex() ];
						const Vec3f & positionOX1 = positions[ OX1->getIndex() ];
						const Vec3f & positionCA2 = positions[ CA2->getIndex() ];

						_addControlPoints( positionCA1,
										   positionOX1,
										   positionCA2,
										   residue1.getSecondaryStructure(),
										   residue1.getHandedness(),
										   flipTestV,
										   splineCenter,
										   splineSide1,
										   splineSide2 );

						splineSide1.copyPoint( 3, 2 );
						splineCenter.copyPoint( 3, 2 );
						splineSide2.copyPoint( 3, 2 );

						const Residue & residue3 = p_molecule.getResidue( idxFirstResidue + residueIdx + 2 );

						const Model::Atom * OX2 = residue2.findFirstAtomByName( "O" );
						const Model::Atom * CA3 = residue3.findFirstAtomByName( "CA" );

						if ( CA2 == nullptr || OX2 == nullptr || CA3 == nullptr )
						{
							// VTX_DEBUG( "Failed to get atoms" );
							continue;
						}

						const Vec3f & positionOX2 = positions[ OX2->getIndex() ];
						const Vec3f & positionCA3 = positions[ CA3->getIndex() ];

						_addControlPoints( positionCA2,
										   positionOX2,
										   positionCA3,
										   residue1.getSecondaryStructure(),
										   residue1.getHandedness(),
										   flipTestV,
										   splineCenter,
										   splineSide1,
										   splineSide2 );

						const Vec3f & p2 = splineCenter.getPoint( 2u );
						const Vec3f & p3 = splineCenter.getPoint( 3u );
						const Vec3f	  p1 = Util::Math::linearComb( 2.f, p2, -1.f, p3 );

						splineCenter.setPoint( 1, p1 );
						splineSide1.setPoint( 1,
											  Util::Math::linearComb( 1.f, p1, RIBBON_WIDTH[ int( ss ) ], flipTestV ) );
						splineSide2.setPoint(
							1, Util::Math::linearComb( 1.f, p1, -RIBBON_WIDTH[ int( ss ) ], flipTestV ) );

						const Vec3f p0 = Util::Math::linearComb( 2.f, p1, -1.f, p2 );

						splineCenter.setPoint( 0, p0 );
						splineSide1.setPoint( 0,
											  Util::Math::linearComb( 1.f, p0, RIBBON_WIDTH[ int( ss ) ], flipTestV ) );
						splineSide2.setPoint(
							0, Util::Math::linearComb( 1.f, p0, -RIBBON_WIDTH[ int( ss ) ], flipTestV ) );
					}
					else
					{
						// Hum?
						/*
						if ( residueIdx > residueCount - 2 )
						{
							continue;
						}
						*/

						splineCenter.shiftPoints();
						splineSide1.shiftPoints();
						splineSide2.shiftPoints();

						if ( residueIdx >= residueCount - 2 )
						{
							const Vec3f & p1 = splineCenter.getPoint( 1u );
							const Vec3f & p2 = splineCenter.getPoint( 2u );

							const Vec3f p3 = Util::Math::linearComb( 2.f, p2, -1.f, p1 );

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
								VTX_WARNING( "Failed to get atoms" );
								continue;
							}

							const Vec3f & positionCA2 = positions[ CA2->getIndex() ];
							const Vec3f & positionOX2 = positions[ OX2->getIndex() ];
							const Vec3f & positionCA3 = positions[ CA3->getIndex() ];

							_addControlPoints( positionCA2,
											   positionOX2,
											   positionCA3,
											   residue1.getSecondaryStructure(),
											   residue1.getHandedness(),
											   flipTestV,
											   splineCenter,
											   splineSide1,
											   splineSide2 );
						}
					}

					// Is arrow if previous == STRAND and current is last or != STRAND.
					bool isArrow
						= ARROW && ( residueIdx > 0 )
						  && ( p_molecule.getResidue( idxFirstResidue + residueIdx + 1 ).getSecondaryStructure()
							   == Residue::SECONDARY_STRUCTURE::STRAND )
						  && ( ( residueIdx == residueCount - 1 )
							   || p_molecule.getResidue( idxFirstResidue + residueIdx ).getSecondaryStructure()
									  != Residue::SECONDARY_STRUCTURE::STRAND );

					_mapResidueIdxToVertexIdx.emplace( residue1.getIndex(), vIndex );
					_computeTriangleMesh( splineCenter, splineSide1, splineSide2, chainColor, isArrow, vIndex );

					residueValidCount++;
				}
			}

			refreshVisibility( p_molecule );

			_vertices.shrink_to_fit();
			_normals.shrink_to_fit();
			_colors.shrink_to_fit();
			_visibilities.shrink_to_fit();
			_indices.shrink_to_fit();

			chrono.stop();
			VTX_INFO( "Secondary structure created in " + std::to_string( chrono.elapsedTime() ) + "s" );
		}

		void Ribbon::_addControlPoints( const Vec3f &					   p_CA0Pos,
										const Vec3f &					   p_OX0Pos,
										const Vec3f &					   p_CA1Pos,
										const Residue::SECONDARY_STRUCTURE p_ss,
										const Residue::HANDEDNESS		   p_handedness,
										Vec3f &							   p_flipTestV,
										Math::BSpline &					   p_splineCenter,
										Math::BSpline &					   p_splineSide1,
										Math::BSpline &					   p_splineSide2 )
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
			if ( ( p_ss != Residue::SECONDARY_STRUCTURE::HELIX ) && ( Util::Math::dot( p_flipTestV, D ) < 0.f ) )
			{
				D = D * -1.f; // flip detected, the plane vector is inverted
			}

			// The central control point is constructed
			cpt0 = Util::Math::linearComb( 0.5f, p_CA0Pos, 0.5f, p_CA1Pos );
			p_splineCenter.setPoint( 3, cpt0 );

			if ( p_ss == Residue::SECONDARY_STRUCTURE::HELIX )
			{
				// When residue i is contained in a helix, the control point is moved away
				// from the helix axis, along the C direction.
				p0	 = p_splineCenter.getPoint( 3u );
				cpt0 = Util::Math::linearComb( 1.f, p0, float( p_handedness ) * HELIX_DIAM, C );
				p_splineCenter.setPoint( 3, cpt0 );
			}

			// The control points for the side ribbons are constructed.
			cpt1 = Util::Math::linearComb( 1.f, cpt0, RIBBON_WIDTH[ int( p_ss ) ], D );
			p_splineSide1.setPoint( 3, cpt1 );

			cpt2 = Util::Math::linearComb( 1.f, cpt0, -RIBBON_WIDTH[ int( p_ss ) ], D );
			p_splineSide2.setPoint( 3, cpt2 );

			// Saving the plane vector (for the flipping test in the next call)
			p_flipTestV = D;
		}

		void Ribbon::_computeTriangleMesh( Math::BSpline &	  p_splineCenter,
										   Math::BSpline &	  p_splineSide1,
										   Math::BSpline &	  p_splineSide2,
										   const Color::Rgb & p_color,
										   const bool		  p_isArrow,
										   uint &			  p_vIndex )
		{
			Vec3f pointCenter0, pointCenter1;
			Vec3f pointSide10, pointSide11, pointSide20, pointSide21;
			Vec3f vecLeft0, vecRight0, vecLeft1, vecRight1;
			Vec3f forward, diagLeft0, diagRight0, diagLeft1, diagRight1;
			Vec3f transversal, tangent, normal0, normal1;
			float t = 0.f;

			// The initial geometry is generated
			pointCenter1 = p_splineCenter.computePoint( 0.f );
			pointSide11	 = p_splineSide1.computePoint( 0.f );
			pointSide21	 = p_splineSide2.computePoint( 0.f );

			// The tangents at the three previous points are the same
			tangent = p_splineSide2.computeTangent( 0.f );

			// Vector transversal to the ribbon
			transversal = pointSide11 - pointSide21;
			normal1		= Util::Math::cross( transversal, tangent );

			Util::Math::normalizeSelf( normal1 );

			Vec3f leftNormal0, leftNormal1, rightNormal0, rightNormal1;

			float extraWidthFactor	 = p_isArrow ? ARROW_WIDTH : 0.f;
			float pointOneAdjustment = p_isArrow ? ARROW_WIDTH / float( DETAIL_LEVEL ) : 0.f;
			float pointOneWidthFactor;

			for ( uint step = 1; step <= DETAIL_LEVEL; ++step )
			{
				t = float( step ) / float( DETAIL_LEVEL );

				pointOneWidthFactor = extraWidthFactor - pointOneAdjustment;

				// The geometry of the previous iteration is saved
				pointSide10	 = pointSide11;
				pointCenter0 = pointCenter1;
				pointSide20	 = pointSide21;
				normal0		 = normal1;

				// The new geometry is generated
				pointSide11	 = p_splineSide1.computePoint( t );
				pointCenter1 = p_splineCenter.computePoint( t );
				pointSide21	 = p_splineSide2.computePoint( t );

				// The tangents at the three previous points are the same
				tangent = p_splineSide2.computeTangent( t );

				// Vector transversal to the ribbon
				transversal = pointSide11 - pointSide21;
				normal1		= Util::Math::cross( transversal, tangent );
				Util::Math::normalizeSelf( normal1 );

				// Left and right may be reversed, but either way,
				// these normals point outwards from the ribbons, horizontally.
				vecLeft0  = pointSide10 - pointCenter0;
				vecLeft1  = pointSide11 - pointCenter1;
				vecRight0 = pointSide20 - pointCenter0;
				vecRight1 = pointSide21 - pointCenter1;

				leftNormal0 = Util::Math::normalize( vecLeft0 );
				leftNormal1 = Util::Math::normalize( vecLeft1 );

				rightNormal0 = Util::Math::normalize( vecRight0 );
				rightNormal1 = Util::Math::normalize( vecRight1 );

				// The (Sid1Point0, Sid1Point1, CentPoint1) triangle is added.
				_vertices.emplace_back( pointSide10 + extraWidthFactor * leftNormal0 ); // 0
				_normals.emplace_back( -normal0 );										// 0

				_vertices.emplace_back( pointSide11 + pointOneWidthFactor * leftNormal1 ); // 1
				_normals.emplace_back( -normal1 );										   // 1

				_vertices.emplace_back( pointCenter1 ); // 2
				_normals.emplace_back( -normal1 );		// 2

				// and duplicated above
				_vertices.emplace_back( pointSide10 + THICKNESS * normal0 + extraWidthFactor * leftNormal0 ); // 3
				_normals.emplace_back( normal0 );															  // 3

				_vertices.emplace_back( pointSide11 + THICKNESS * normal1 + pointOneWidthFactor * leftNormal1 ); // 4
				_normals.emplace_back( normal1 );																 // 4

				_vertices.emplace_back( pointCenter1 + THICKNESS * normal1 ); // 5
				_normals.emplace_back( normal1 );							  // 5

				// The (Sid1Point0, CentPoint1, CentPoint0) triangle is added.
				_vertices.emplace_back( pointSide10 + extraWidthFactor * leftNormal0 ); // 6
				_normals.emplace_back( -normal0 );										// 6

				_vertices.emplace_back( pointCenter1 ); // 7
				_normals.emplace_back( -normal1 );		// 7

				_vertices.emplace_back( pointCenter0 ); // 8
				_normals.emplace_back( -normal0 );		// 8

				// and duplicated above
				_vertices.emplace_back( pointSide10 + THICKNESS * normal0 + extraWidthFactor * leftNormal0 ); // 9
				_normals.emplace_back( normal0 );															  // 9

				_vertices.emplace_back( pointCenter1 + THICKNESS * normal1 ); // 10
				_normals.emplace_back( normal1 );							  // 10

				_vertices.emplace_back( pointCenter0 + THICKNESS * normal0 ); // 11
				_normals.emplace_back( normal0 );							  // 11

				// (Sid2Point0, Sid2Point1, CentPoint1) triangle is added.
				_vertices.emplace_back( pointSide20 + extraWidthFactor * rightNormal0 ); // 12
				_normals.emplace_back( -normal0 );										 // 12

				_vertices.emplace_back( pointSide21 + pointOneWidthFactor * rightNormal1 ); // 13
				_normals.emplace_back( -normal1 );											// 13

				_vertices.emplace_back( pointCenter1 ); // 14
				_normals.emplace_back( -normal1 );		// 14

				// and duplicated above
				_vertices.emplace_back( pointSide20 + THICKNESS * normal0 + extraWidthFactor * rightNormal0 ); // 15
				_normals.emplace_back( normal0 );															   // 15

				_vertices.emplace_back( pointSide21 + THICKNESS * normal1 + pointOneWidthFactor * rightNormal1 ); // 16
				_normals.emplace_back( normal1 );																  // 16

				_vertices.emplace_back( pointCenter1 + THICKNESS * normal1 ); // 17
				_normals.emplace_back( normal1 );							  // 17

				// (Sid2Point0, CentPoint1, CentPoint0) triangle is added.
				_vertices.emplace_back( pointSide20 + extraWidthFactor * rightNormal0 ); // 18
				_normals.emplace_back( -normal0 );										 // 18

				_vertices.emplace_back( pointCenter1 ); // 19
				_normals.emplace_back( -normal1 );		// 19

				_vertices.emplace_back( pointCenter0 ); // 20
				_normals.emplace_back( -normal0 );		// 20

				// and duplicated above
				_vertices.emplace_back( pointSide20 + THICKNESS * normal0 + extraWidthFactor * rightNormal0 ); // 21
				_normals.emplace_back( normal0 );															   // 21

				_vertices.emplace_back( pointCenter1 + THICKNESS * normal1 ); // 22
				_normals.emplace_back( normal1 );							  // 22

				_vertices.emplace_back( pointCenter0 + THICKNESS * normal0 ); // 23
				_normals.emplace_back( normal0 );							  // 23

				// Duplicating the side vertices and giving them the proper normals
				// for the sides of the thick ribbons
				_vertices.emplace_back( pointSide10 + extraWidthFactor * leftNormal0 );		// 24
				_vertices.emplace_back( pointSide11 + pointOneWidthFactor * leftNormal1 );	// 25
				_vertices.emplace_back( pointSide20 + extraWidthFactor * rightNormal0 );	// 26
				_vertices.emplace_back( pointSide21 + pointOneWidthFactor * rightNormal1 ); // 27

				// and duplicating them again, this time raising them as well
				_vertices.emplace_back( pointSide10 + THICKNESS * normal0 + extraWidthFactor * leftNormal0 );	  // 28
				_vertices.emplace_back( pointSide11 + THICKNESS * normal1 + pointOneWidthFactor * leftNormal1 );  // 29
				_vertices.emplace_back( pointSide20 + THICKNESS * normal0 + extraWidthFactor * rightNormal0 );	  // 30
				_vertices.emplace_back( pointSide21 + THICKNESS * normal1 + pointOneWidthFactor * rightNormal1 ); // 31

				if ( p_isArrow )
				{
					forward				= pointCenter1 - pointCenter0;
					float forwardLength = Util::Math::length( forward );

					diagLeft0  = Util::Math::normalize( forwardLength * forward
														+ ( Util::Math::length( vecLeft0 ) + ARROW_WIDTH ) * vecLeft0 );
					diagLeft1  = Util::Math::normalize( forwardLength * forward
														+ ( Util::Math::length( vecLeft1 ) + ARROW_WIDTH ) * vecLeft1 );
					diagRight0 = Util::Math::normalize(
						forwardLength * forward + ( Util::Math::length( vecRight0 ) + ARROW_WIDTH ) * vecRight0 );
					diagRight1 = Util::Math::normalize(
						forwardLength * forward + ( Util::Math::length( vecRight1 ) + ARROW_WIDTH ) * vecRight1 );

					_normals.emplace_back( diagLeft0 );	 // 24
					_normals.emplace_back( diagLeft1 );	 // 25
					_normals.emplace_back( diagRight0 ); // 26
					_normals.emplace_back( diagRight1 ); // 27
					_normals.emplace_back( diagLeft0 );	 // 28
					_normals.emplace_back( diagLeft1 );	 // 29
					_normals.emplace_back( diagRight0 ); // 30
					_normals.emplace_back( diagRight1 ); // 31
				}
				else
				{
					_normals.emplace_back( leftNormal0 );  // 24
					_normals.emplace_back( leftNormal1 );  // 25
					_normals.emplace_back( rightNormal0 ); // 26
					_normals.emplace_back( rightNormal1 ); // 27
					_normals.emplace_back( leftNormal0 );  // 28
					_normals.emplace_back( leftNormal1 );  // 29
					_normals.emplace_back( rightNormal0 ); // 30
					_normals.emplace_back( rightNormal1 ); // 31
				}

				// Colors.
				for ( uint t = 0; t < 32; ++t )
				{
					_colors.emplace_back( p_color );
				}

				// Triangle indices.
				for ( uint t = 0; t < 24; ++t )
				{
					_indices.emplace_back( p_vIndex + t );
				}

				_indices.emplace_back( p_vIndex + 24 );
				_indices.emplace_back( p_vIndex + 25 );
				_indices.emplace_back( p_vIndex + 28 );

				_indices.emplace_back( p_vIndex + 25 );
				_indices.emplace_back( p_vIndex + 28 );
				_indices.emplace_back( p_vIndex + 29 );

				_indices.emplace_back( p_vIndex + 26 );
				_indices.emplace_back( p_vIndex + 27 );
				_indices.emplace_back( p_vIndex + 30 );

				_indices.emplace_back( p_vIndex + 27 );
				_indices.emplace_back( p_vIndex + 30 );
				_indices.emplace_back( p_vIndex + 31 );

				if ( p_isArrow )
				{
					_indices.emplace_back( p_vIndex + 24 );
					_indices.emplace_back( p_vIndex + 26 );
					_indices.emplace_back( p_vIndex + 28 );

					_indices.emplace_back( p_vIndex + 26 );
					_indices.emplace_back( p_vIndex + 28 );
					_indices.emplace_back( p_vIndex + 30 );
				}

				p_vIndex += 32;
				extraWidthFactor = extraWidthFactor - pointOneAdjustment;
			}
		} // namespace Model

		void Ribbon::refreshVisibility( const Molecule & p_molecule )
		{
			_visibilities.resize( _vertices.size() );
			uint count = 0;

			for ( Chain * const chain : p_molecule.getChains() )
			{
				for ( uint i = 0; i < chain->getResidueCount(); ++i )
				{
					const Residue & residue = p_molecule.getResidue( chain->getIdFirstResidue() + i );
					bool			show	= p_molecule.isVisible() && chain->isVisible() && residue.isVisible();

					if ( _mapResidueIdxToVertexIdx.find( residue.getIndex() ) != _mapResidueIdxToVertexIdx.end() )
					{
						for ( uint j = 0; j < 32 * DETAIL_LEVEL; ++j, ++count )
						{
							_visibilities[ _mapResidueIdxToVertexIdx[ residue.getIndex() ] + j ] = show;
						}
					}
				}
			}

			if ( _vertices.size() != count )
			{
				VTX_WARNING( "VBO size missmatch: " + std::to_string( _vertices.size() ) + " / "
							 + std::to_string( count ) );
			}

			glNamedBufferData(
				_vboVisibilities, sizeof( uint ) * _visibilities.size(), _visibilities.data(), GL_STATIC_DRAW );
		}

	} // namespace Model
} // namespace VTX
