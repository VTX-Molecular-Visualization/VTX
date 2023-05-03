#include "app/util/secondary_structure.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/component/chemistry/secondary_structure.hpp"
#include "app/internal/chemdb/secondary_structure.hpp"
#include <util/chrono.hpp>
#include <util/logger.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::Util::App::SecondaryStructure
{
	void computeSecondaryStructure( VTX::App::Component::Chemistry::Molecule & p_molecule )
	{
		Util::Chrono chrono;
		chrono.start();
		VTX_INFO( "Computing secondary structure..." );

		computeStride( p_molecule );

		p_molecule.getConfiguration().isSecondaryStructureLoadedFromFile = false;
		chrono.stop();
		VTX_INFO( "Secondary structure computed in " + std::to_string( chrono.elapsedTime() ) + "s" );
	}

	void computeStride( VTX::App::Component::Chemistry::Molecule & p_molecule )
	{
		const VTX::App::Component::Chemistry::Molecule::AtomPositionsFrame & positions
			= p_molecule.getAtomPositionFrame( p_molecule.getFrame() );

		for ( uint chainIdx = 0; chainIdx < p_molecule.getChainCount(); ++chainIdx )
		{
			const VTX::App::Component::Chemistry::Chain * const chainPtr = p_molecule.getChain( chainIdx );

			if ( chainPtr == nullptr )
				continue;

			const VTX::App::Component::Chemistry::Chain & chain		   = *chainPtr;
			const uint									  residueCount = chain.getResidueCount();

			// Not enought residues.
			if ( residueCount < 4 )
			{
				VTX_DEBUG( "Chain residue count < 4" );
				continue;
			}

			uint			   idxFirstResidue = chain.getIndexFirstResidue();
			std::vector<float> phi			   = std::vector<float>( residueCount );
			std::vector<float> psi			   = std::vector<float>( residueCount );
			// std::vector<float> omega		   = std::vector<float>( residueCount );

			// Compute dihedral angles psi and phi.

			// Terminal values.
			phi[ 0 ]				= PI_2f;
			psi[ 0 ]				= PI_2f;
			phi[ residueCount - 1 ] = PI_2f;
			psi[ residueCount - 1 ] = PI_2f;

			p_molecule.getResidue( idxFirstResidue )->setSecondaryStructure( ChemDB::SecondaryStructure::TYPE::COIL );
			p_molecule.getResidue( idxFirstResidue + residueCount - 1 )
				->setSecondaryStructure( ChemDB::SecondaryStructure::TYPE::COIL );

			for ( uint residueIdx = 1; residueIdx < residueCount - 1; ++residueIdx )
			{
				const uint currentResidueIndex = idxFirstResidue + residueIdx;
				const VTX::App::Component::Chemistry::Residue * const residue0Ptr
					= p_molecule.getResidue( currentResidueIndex - 1 );
				VTX::App::Component::Chemistry::Residue * const residue1Ptr
					= p_molecule.getResidue( currentResidueIndex );
				const VTX::App::Component::Chemistry::Residue * const residue2Ptr
					= p_molecule.getResidue( currentResidueIndex + 1 );

				if ( residue0Ptr == nullptr || residue1Ptr == nullptr || residue2Ptr == nullptr )
				{
					continue;
				}

				const VTX::App::Component::Chemistry::Residue & residue0 = *residue0Ptr;
				VTX::App::Component::Chemistry::Residue &		residue1 = *residue1Ptr;
				const VTX::App::Component::Chemistry::Residue & residue2 = *residue2Ptr;

				residue1.setSecondaryStructure( ChemDB::SecondaryStructure::TYPE::COIL );

				const VTX::App::Component::Chemistry::Atom * C0	 = residue0.findFirstAtomByName( "C" );
				const VTX::App::Component::Chemistry::Atom * N1	 = residue1.findFirstAtomByName( "N" );
				const VTX::App::Component::Chemistry::Atom * CA1 = residue1.getAlphaCarbon();
				const VTX::App::Component::Chemistry::Atom * C1	 = residue1.findFirstAtomByName( "C" );
				const VTX::App::Component::Chemistry::Atom * N2	 = residue2.findFirstAtomByName( "N" );

				if ( C0 == nullptr || N1 == nullptr || CA1 == nullptr || C1 == nullptr || N2 == nullptr )
				{
					// VTX_DEBUG( "Failed to get atoms in chain: " + chain.getName() );
					continue;
				}

				const Vec3f & positionC0  = positions[ C0->getIndex() ];
				const Vec3f & positionN1  = positions[ N1->getIndex() ];
				const Vec3f & positionCA1 = positions[ CA1->getIndex() ];
				const Vec3f & positionC1  = positions[ C1->getIndex() ];
				const Vec3f & positionN2  = positions[ N2->getIndex() ];

				phi[ residueIdx ] = Util::Math::torsionalAngle( positionC0, positionN1, positionCA1, positionC1 );
				psi[ residueIdx ] = Util::Math::torsionalAngle( positionN1, positionCA1, positionC1, positionN2 );
			}

			uint firstHelixIdx	= 0u;
			uint RHelixCount	= 0u;
			uint LHelixCount	= 0u;
			uint firstStrandIdx = 0u;
			uint strandCount	= 0u;

			for ( uint residueIdx = 0; residueIdx < residueCount; ++residueIdx )
			{
				// Right-handed helix
				if ( ( Util::Math::distance( Vec2f( phi[ residueIdx ], psi[ residueIdx ] ),
											 Vec2f( -PIf / 3.f, -PIf / 4.f ) )
					   < ( PIf / 6.f ) )
					 && ( residueIdx < residueCount - 1 ) )
				{
					if ( RHelixCount == 0 )
					{
						firstHelixIdx = residueIdx;
					}
					RHelixCount++;
				}
				else
				{
					if ( 3 <= RHelixCount )
					{
						for ( uint k = firstHelixIdx; k < residueIdx; k++ )
						{
							VTX::App::Component::Chemistry::Residue & residue
								= *p_molecule.getResidue( idxFirstResidue + k );
							residue.setSecondaryStructure( ChemDB::SecondaryStructure::TYPE::HELIX_ALPHA_RIGHT );
						}
					}
					RHelixCount = 0;
				}

				// Left-handed helix
				if ( ( Util::Math::distance( Vec2f( phi[ residueIdx ], psi[ residueIdx ] ),
											 Vec2f( PIf / 3.f, PIf / 4.f ) )
					   < ( PIf / 6.f ) )
					 && ( residueIdx < residueCount - 1 ) )
				{
					if ( LHelixCount == 0 )
					{
						firstHelixIdx = residueIdx;
					}
					LHelixCount++;
				}
				else
				{
					if ( 3 <= LHelixCount )
					{
						for ( uint k = firstHelixIdx; k < residueIdx; k++ )
						{
							VTX::App::Component::Chemistry::Residue & residue
								= *p_molecule.getResidue( idxFirstResidue + k );
							residue.setSecondaryStructure( ChemDB::SecondaryStructure::TYPE::HELIX_ALPHA_LEFT );
						}
					}
					LHelixCount = 0;
				}

				// Strand
				if ( ( Util::Math::distance( Vec2f( phi[ residueIdx ], psi[ residueIdx ] ),
											 Vec2f( -Util::Math::radians( 110.f ), Util::Math::radians( 130.f ) ) )
					   < ( PIf / 6.f ) )
					 && ( residueIdx < residueCount - 1 ) )
				{
					if ( strandCount == 0 )
					{
						firstStrandIdx = residueIdx;
					}
					strandCount++;
				}
				else
				{
					if ( 2 <= strandCount )
					{
						for ( uint k = firstStrandIdx; k < residueIdx; k++ )
						{
							VTX::App::Component::Chemistry::Residue & residue
								= *p_molecule.getResidue( idxFirstResidue + k );
							residue.setSecondaryStructure( ChemDB::SecondaryStructure::TYPE::STRAND );
						}
					}
					strandCount = 0;
				}
			}

			for ( uint residueIdx = 1; residueIdx < residueCount - 1; ++residueIdx )
			{
				const uint currentResidueIndex = idxFirstResidue + residueIdx;
				const VTX::App::Component::Chemistry::Residue * const residue0Ptr
					= p_molecule.getResidue( currentResidueIndex - 1 );
				VTX::App::Component::Chemistry::Residue * const residue1Ptr
					= p_molecule.getResidue( currentResidueIndex );
				const VTX::App::Component::Chemistry::Residue * const residue2Ptr
					= p_molecule.getResidue( currentResidueIndex + 1 );

				if ( residue0Ptr == nullptr || residue1Ptr == nullptr || residue2Ptr == nullptr )
				{
					continue;
				}

				const VTX::App::Component::Chemistry::Residue & residue0 = *residue0Ptr;
				VTX::App::Component::Chemistry::Residue &		residue	 = *residue1Ptr;
				const VTX::App::Component::Chemistry::Residue & residue2 = *residue2Ptr;

				if ( ( residue0.getSecondaryStructure() == residue2.getSecondaryStructure() )
					 && ( ( residue0.getSecondaryStructure() == ChemDB::SecondaryStructure::TYPE::HELIX_ALPHA_RIGHT )
						  || ( residue0.getSecondaryStructure() == ChemDB::SecondaryStructure::TYPE::HELIX_ALPHA_LEFT )
						  || ( residue0.getSecondaryStructure() == ChemDB::SecondaryStructure::TYPE::STRAND ) ) )

				{
					residue.setSecondaryStructure( residue0.getSecondaryStructure() );
				}
			}
		}
	}

	ChemDB::SecondaryStructure::TYPE pdbFormattedToEnumSecondaryStructure( const std::string & p_str )
	{
		ChemDB::SecondaryStructure::TYPE res = ChemDB::SecondaryStructure::TYPE::COUNT;

		if ( p_str == "extended" )
			res = ChemDB::SecondaryStructure::TYPE::STRAND;
		else if ( p_str == "turn" )
			res = ChemDB::SecondaryStructure::TYPE::TURN;
		else if ( p_str == "coil" )
			res = ChemDB::SecondaryStructure::TYPE::COIL;
		else if ( p_str == "right-handed alpha helix" )
			res = ChemDB::SecondaryStructure::TYPE::HELIX_ALPHA_RIGHT;
		else if ( p_str == "left-handed alpha helix" )
			res = ChemDB::SecondaryStructure::TYPE::HELIX_ALPHA_LEFT;
		else if ( p_str == "right-handed 3-10 helix" )
			res = ChemDB::SecondaryStructure::TYPE::HELIX_3_10_RIGHT;
		else if ( p_str == "left-handed 3-10 helix" )
			res = ChemDB::SecondaryStructure::TYPE::HELIX_3_10_LEFT;
		else if ( p_str == "pi helix" )
			res = ChemDB::SecondaryStructure::TYPE::HELIX_PI;
		else if ( p_str == "right-handed omega helix" ) // ?
			res = ChemDB::SecondaryStructure::TYPE::COUNT;
		else if ( p_str == "left-handed omega helix" ) // ?
			res = ChemDB::SecondaryStructure::TYPE::COUNT;
		else if ( p_str == "right-handed gamma helix" ) // ?
			res = ChemDB::SecondaryStructure::TYPE::COUNT;
		else if ( p_str == "left-handed gamma helix" ) // ?
			res = ChemDB::SecondaryStructure::TYPE::COUNT;
		else if ( p_str == "2 - 7 ribbon / helix" ) // ?
			res = ChemDB::SecondaryStructure::TYPE::COUNT;
		else if ( p_str == "polyproline" ) // ?
			res = ChemDB::SecondaryStructure::TYPE::COUNT;

		return res;
	}

	std::string enumToPdbFormattedSecondaryStructure( const ChemDB::SecondaryStructure::TYPE p_enum )
	{
		std::string res;

		switch ( p_enum )
		{
		case ChemDB::SecondaryStructure::TYPE::STRAND: res = "extended"; break;
		case ChemDB::SecondaryStructure::TYPE::TURN: res = "turn"; break;
		case ChemDB::SecondaryStructure::TYPE::COIL: res = "coil"; break;
		case ChemDB::SecondaryStructure::TYPE::HELIX_ALPHA_RIGHT: res = "right-handed alpha helix"; break;
		case ChemDB::SecondaryStructure::TYPE::HELIX_ALPHA_LEFT: res = "left-handed alpha helix"; break;
		case ChemDB::SecondaryStructure::TYPE::HELIX_3_10_RIGHT: res = "right-handed 3-10 helix"; break;
		case ChemDB::SecondaryStructure::TYPE::HELIX_3_10_LEFT: res = "left-handed 3-10 helix"; break;
		case ChemDB::SecondaryStructure::TYPE::HELIX_PI: res = "pi helix"; break;
		default: res = ""; break;
		}

		return res;
	}

} // namespace VTX::Util::App::SecondaryStructure
