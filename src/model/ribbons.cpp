#include "ribbons.hpp"

namespace VTX
{
	namespace Model
	{
		Ribbons::Ribbons( const Molecule & p_molecule )
		{
			VTX_DEBUG( "Building secondary structure..." );

			const Molecule::AtomPositionsFrame & positions = p_molecule.getAtomPositionFrame( 0 );

			// Loop over chains.
			for ( uint chainIdx = 0; chainIdx < p_molecule.getChainCount(); ++chainIdx )
			{
				VTX_DEBUG( "Building secondary structure... chain " + std::to_string( chainIdx ) );
				const Chain & chain			  = p_molecule.getChain( chainIdx );
				uint		  residueCount	  = chain.getResidueCount();
				uint		  idxFirstResidue = chain.getIdFirstResidue();

				Vec3f flipTestV = VEC3F_ZERO;

				Math::BSpline splineCenter = Math::BSpline( 4 );
				Math::BSpline splineSide1  = Math::BSpline( 4 );
				Math::BSpline splineSide2  = Math::BSpline( 4 );

				// Loop over residues
				for ( uint residueIdx = 0; residueIdx < residueCount; ++residueIdx )
				{
					VTX_DEBUG( "Building secondary structure... residue " + std::to_string( residueIdx ) );
					const Residue & residue = p_molecule.getResidue( idxFirstResidue + residueIdx );

					if ( residueIdx == 0 )
					{
						// flipTestV				 = VEC3D_ZERO;
						const Residue & residue2 = p_molecule.getResidue( idxFirstResidue + 1 );

						const Model::Atom * CA1 = residue.findFirstAtomByName( "CA" );
						const Model::Atom * OX1 = residue.findFirstAtomByName( "O" );
						const Model::Atom * CA2 = residue2.findFirstAtomByName( "CA" );

						if ( CA1 == nullptr || OX1 == nullptr || CA2 == nullptr )
						{
							VTX_ERROR( "Failed to get atoms for peptide plane" );
							return;
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
							VTX_ERROR( "Failed to get atoms for peptide plane" );
							return;
						}

						const Vec3f & positionOX2 = positions[ OX2->getIndex() ];
						const Vec3f & positionCA3 = positions[ CA3->getIndex() ];

						_addControlPoints( positionCA2,
										   positionOX2,
										   positionCA3,
										   residue.getSecondaryStructure(),
										   flipTestV,
										   splineCenter,
										   splineCenter,
										   splineSide2 );

						Vec3f p2 = splineCenter.getPoint( 2 );
						Vec3f p3 = splineCenter.getPoint( 3 );
						Vec3f p1 = Util::Math::linearComb( 2.f, p2, -1.f, p3 );

						splineCenter.setPoint( 1, p1 );
						splineSide1.setPoint( 1,
											  Util::Math::linearComb( 1.f, p1, /*ribbonWidth[ ss ]*/ 1.f, flipTestV ) );
						splineSide2.setPoint(
							1, Util::Math::linearComb( 1.f, p1, /*-ribbonWidth[ ss ]*/ 1.f, flipTestV ) );

						Vec3f p0 = Util::Math::linearComb( 2.f, p1, -1.f, p2 );

						splineCenter.setPoint( 0, p0 );
						splineSide1.setPoint( 0,
											  Util::Math::linearComb( 1.f, p0, /*ribbonWidth[ ss ]*/ 1.f, flipTestV ) );
						splineSide2.setPoint(
							0, Util::Math::linearComb( 1.f, p0, /*-ribbonWidth[ ss ]*/ 1.f, flipTestV ) );
					}
					else
					{
						splineCenter.shiftPoints();
						splineSide1.shiftPoints();
						splineSide2.shiftPoints();

						if ( ( residueIdx == residueCount - 1 ) || ( residueIdx == residueCount - 2 ) )
						{
							Vec3f p1 = splineCenter.getPoint( 1 );
							Vec3f p2 = splineCenter.getPoint( 2 );

							Vec3f p3 = Util::Math::linearComb( 2.f, p2, -1.f, p1 );

							splineCenter.setPoint( 3, p3 );
							splineSide1.setPoint(
								3, Util::Math::linearComb( 1.f, p3, /*ribbonWidth[ ss ]*/ 1.f, flipTestV ) );
							splineSide2.setPoint(
								3, Util::Math::linearComb( 1.f, p3, /*-ribbonWidth[ ss ]*/ 1.f, flipTestV ) );
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
								VTX_ERROR( "Failed to get atoms for peptide plane" );
								return;
							}

							const Vec3f & positionCA1 = positions[ CA2->getIndex() ];
							const Vec3f & positionOX1 = positions[ OX2->getIndex() ];
							const Vec3f & positionCA2 = positions[ CA3->getIndex() ];

							_addControlPoints( positionCA1,
											   positionOX1,
											   positionCA2,
											   residue.getSecondaryStructure(),
											   flipTestV,
											   splineCenter,
											   splineCenter,
											   splineSide2 );
						}
					}
				}
			}
		}

		// TODO: merge that with PeptidePlane class?
		void Ribbons::_addControlPoints( const Vec3f &						p_CA0Pos,
										 const Vec3f &						p_OX0Pos,
										 const Vec3f &						p_CA1Pos,
										 const Residue::SECONDARY_STRUCTURE p_ss,
										 Vec3f &							p_flipTestV,
										 Math::BSpline &					p_splineCenter,
										 Math::BSpline &					p_splineSide1,
										 Math::BSpline &					p_splineSide2 )
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
			if ( ( p_ss != Residue::SECONDARY_STRUCTURE::HELIX ) && ( 90.f < Util::Math::angle( p_flipTestV, D ) ) )
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
				p0	 = p_splineCenter.getPoint( 3 );
				cpt0 = Util::Math::linearComb( 1.f, p0, /*handedness **/ HELIX_DIAM, C );
				p_splineCenter.setPoint( 3, cpt0 );
			}

			// The control points for the side ribbons are constructed.
			cpt1 = Util::Math::linearComb( 1.f, cpt0, RIBBON_WIDTH[ (int)p_ss ], D );
			p_splineSide1.setPoint( 3, cpt1 );

			cpt2 = Util::Math::linearComb( 1.f, cpt0, -RIBBON_WIDTH[ (int)p_ss ], D );
			p_splineSide2.setPoint( 3, cpt2 );

			// Saving the plane vector (for the flipping test in the next call)
			p_flipTestV = D;
		}

	} // namespace Model
} // namespace VTX
