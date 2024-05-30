#include "io/util/secondary_structure.hpp"
#include <util/logger.hpp>
#include <util/math.hpp>

namespace VTX::IO::Util::SecondaryStructure
{

	void computeStride( Core::Struct::System & p_system )
	{
		using namespace VTX::Util;

		// devjla
		// const Core::Struct::Frame & positions = p_system.trajectory.getCurrentFrame();
		Core::Struct::Frame positions { { FLOAT_MAX, FLOAT_MAX, FLOAT_MAX } };
		if ( !p_system.trajectory.getCurrentFrame( positions ) )
		{
			// FIXME ouch
			VTX_ERROR( "DEVJLA current frame issue" );
			return;
		}

		std::vector<Core::ChemDB::SecondaryStructure::TYPE> & types = p_system.residueSecondaryStructureTypes;

		for ( uint chainIdx = 0; chainIdx < p_system.getChainCount(); ++chainIdx )
		{
			/*
			const Model::Chain * const chainPtr = p_system.getChain( chainIdx );

			if ( chainPtr == nullptr )
				continue;


			const Model::Chain & chain		  = *chainPtr;
			*/

			const size_t residueCount = p_system.chainResidueCounts[ chainIdx ];

			// Not enought atoms.
			if ( residueCount < 4 )
			{
				VTX_DEBUG( "Chain residue count < 4" );
				continue;
			}

			const size_t	   idxFirstResidue = p_system.chainFirstResidues[ chainIdx ];
			std::vector<float> phi			   = std::vector<float>( residueCount );
			std::vector<float> psi			   = std::vector<float>( residueCount );
			// std::vector<float> omega		   = std::vector<float>( residueCount );

			// Compute dihedral angles psi and phi.

			// Terminal values.
			phi[ 0 ]				= PI_2f;
			psi[ 0 ]				= PI_2f;
			phi[ residueCount - 1 ] = PI_2f;
			psi[ residueCount - 1 ] = PI_2f;

			types[ idxFirstResidue ]					= Core::ChemDB::SecondaryStructure::TYPE::COIL;
			types[ idxFirstResidue + residueCount - 1 ] = Core::ChemDB::SecondaryStructure::TYPE::COIL;

			auto findFirstAtomByName = [ &p_system ]( const size_t p_residueIdx, const std::string & p_name )
			{
				const atom_index_t atomCount	= p_system.residueAtomCounts[ p_residueIdx ];
				const atom_index_t idxFirstAtom = p_system.residueFirstAtomIndexes[ p_residueIdx ];

				for ( int i = idxFirstAtom; i < int( idxFirstAtom + atomCount ); ++i )
				{
					if ( p_system.atomNames[ i ] == p_name )
					{
						return i;
					}
				}

				return -1;
			};

			for ( uint residueIdx = 1; residueIdx < residueCount - 1; ++residueIdx )
			{
				const size_t currentResidueIndex = idxFirstResidue + residueIdx;

				/*
				if ( residue0Ptr == nullptr || residue1Ptr == nullptr || residue2Ptr == nullptr )
				{
					continue;
				}
				*/

				const size_t residue0 = currentResidueIndex - 1;
				const size_t residue1 = currentResidueIndex;
				const size_t residue2 = currentResidueIndex + 1;

				types[ residue1 ] = Core::ChemDB::SecondaryStructure::TYPE::COIL;

				const int C0  = findFirstAtomByName( residue0, "C" );
				const int N1  = findFirstAtomByName( residue1, "N" );
				const int CA1 = findFirstAtomByName( residue1, "CA" );
				const int C1  = findFirstAtomByName( residue1, "C" );
				const int N2  = findFirstAtomByName( residue2, "N" );

				if ( C0 == -1 || N1 == -1 || CA1 == -1 || C1 == -1 || N2 == -1 )
				{
					continue;
				}

				const Vec3f & positionC0  = positions[ C0 ];
				const Vec3f & positionN1  = positions[ N1 ];
				const Vec3f & positionCA1 = positions[ CA1 ];
				const Vec3f & positionC1  = positions[ C1 ];
				const Vec3f & positionN2  = positions[ N2 ];

				phi[ residueIdx ] = Math::torsionalAngle( positionC0, positionN1, positionCA1, positionC1 );
				psi[ residueIdx ] = Math::torsionalAngle( positionN1, positionCA1, positionC1, positionN2 );
			}

			size_t firstHelixIdx  = 0;
			size_t RHelixCount	  = 0;
			size_t LHelixCount	  = 0;
			size_t firstStrandIdx = 0;
			size_t strandCount	  = 0;

			for ( size_t residueIdx = 0; residueIdx < residueCount; ++residueIdx )
			{
				// Right-handed helix
				if ( ( Math::distance( Vec2f( phi[ residueIdx ], psi[ residueIdx ] ), Vec2f( -PIf / 3.f, -PIf / 4.f ) )
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
						for ( size_t k = firstHelixIdx; k < residueIdx; k++ )
						{
							types[ idxFirstResidue + k ] = Core::ChemDB::SecondaryStructure::TYPE::HELIX_ALPHA_RIGHT;
						}
					}
					RHelixCount = 0;
				}

				// Left-handed helix
				if ( ( Math::distance( Vec2f( phi[ residueIdx ], psi[ residueIdx ] ), Vec2f( PIf / 3.f, PIf / 4.f ) )
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
						for ( size_t k = firstHelixIdx; k < residueIdx; k++ )
						{
							types[ idxFirstResidue + k ] = Core::ChemDB::SecondaryStructure::TYPE::HELIX_ALPHA_LEFT;
						}
					}
					LHelixCount = 0;
				}

				// Strand
				if ( ( Math::distance(
						   Vec2f( phi[ residueIdx ], psi[ residueIdx ] ),
						   Vec2f( -Math::radians( 110.f ), Math::radians( 130.f ) )
					   )
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
						for ( size_t k = firstStrandIdx; k < residueIdx; k++ )
						{
							types[ idxFirstResidue + k ] = Core::ChemDB::SecondaryStructure::TYPE::STRAND;
						}
					}
					strandCount = 0;
				}
			}

			for ( size_t residueIdx = 1; residueIdx < residueCount - 1; ++residueIdx )
			{
				const size_t currentResidueIndex = idxFirstResidue + residueIdx;

				/*
				if ( residue0Ptr == nullptr || residue1Ptr == nullptr || residue2Ptr == nullptr )
				{
					continue;
				}
				*/

				const size_t residue0 = currentResidueIndex - 1;
				const size_t residue1 = currentResidueIndex;
				const size_t residue2 = currentResidueIndex + 1;

				if ( ( types[ residue0 ] == types[ residue2 ] )
					 && ( ( types[ residue0 ] == Core::ChemDB::SecondaryStructure::TYPE::HELIX_ALPHA_RIGHT )
						  || ( types[ residue0 ] == Core::ChemDB::SecondaryStructure::TYPE::HELIX_ALPHA_LEFT )
						  || ( types[ residue0 ] == Core::ChemDB::SecondaryStructure::TYPE::STRAND ) ) )

				{
					types[ residue1 ] = types[ residue0 ];
				}
			}
		}
	}

} // namespace VTX::IO::Util::SecondaryStructure
