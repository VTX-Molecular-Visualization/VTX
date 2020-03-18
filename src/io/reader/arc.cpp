#include "arc.hpp"
#undef INFINITE
#include "define.hpp"
#include "util/color.hpp"
#include <chemfiles.hpp>

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			bool ARC::readFile( const Path & p_path, Model::Molecule & p_molecule )
			{
				chemfiles::Trajectory trajectory( p_path );
				VTX_INFO( std::to_string( trajectory.nsteps() ) + " frames" );

				if ( trajectory.nsteps() == 0 ) { return false; }

				chemfiles::Frame					 frame	   = trajectory.read();
				chemfiles::span<chemfiles::Vector3D> positions = frame.positions();

				p_molecule.setName( "TEST MD" );
				p_molecule.addChains( 1 );
				Model::Chain & chain = p_molecule.getChain( 0 );
				chain.setMoleculePtr( &p_molecule );
				chain.setIdFirstResidue( 0 );
				chain.setResidueCount( 1 );
				chain.setColor( Util::Color::randomPastelColor() );

				p_molecule.addResidues( 1 );
				Model::Residue & residue = p_molecule.getResidue( 0 );
				residue.setMoleculePtr( &p_molecule );
				residue.setChainPtr( &chain );
				residue.setIdFirstAtom( 0 );
				residue.setAtomCount( uint( positions.size() ) );
				residue.setColor( Util::Color::randomPastelColor() );
				p_molecule.addAtoms( uint( positions.size() ) );

				// First frame.
				VTX_DEBUG( "Frame 0" );
				p_molecule.addAtomPositionFrame();
				Model::Molecule::AtomPositionsFrame & firstMoleculeFrame = p_molecule.getAtomPositionFrame( 0 );
				for ( uint positionIdx = 0; positionIdx < positions.size(); ++positionIdx )
				{
					Model::Atom & atom = p_molecule.getAtom( positionIdx );
					atom.setMoleculePtr( &p_molecule );
					atom.setChainPtr( &chain );
					atom.setResiduePtr( &residue );
					atom.setColor( Util::Color::randomPastelColor() );
					p_molecule.addAtomRadius( 1.20f );
					chemfiles::Vector3D & position = positions[ positionIdx ];
					firstMoleculeFrame.emplace_back( Vec3f( position[ 0 ], position[ 1 ], position[ 2 ] ) );
					const Vec3f & atomPosition = firstMoleculeFrame[ positionIdx ];
					const float	  atomRadius   = 1.20f;
					p_molecule.extendAABB( atomPosition, atomRadius );
				}

				// Fill other frames.
				for ( uint frameIdx = 1; frameIdx < trajectory.nsteps(); ++frameIdx )
				{
					p_molecule.addAtomPositionFrame();
					VTX_DEBUG( "Frame " + std::to_string( frameIdx ) );
					Model::Molecule::AtomPositionsFrame & moleculeFrame = p_molecule.getAtomPositionFrame( frameIdx );

					frame	  = trajectory.read_step( frameIdx );
					positions = frame.positions();

					for ( uint positionIdx = 0; positionIdx < positions.size(); ++positionIdx )
					{
						chemfiles::Vector3D & position = positions[ positionIdx ];
						moleculeFrame.emplace_back( Vec3f( position[ 0 ], position[ 1 ], position[ 2 ] ) );
					}
				}

				return true;
			}

			bool ARC::readBuffer( const std::string & p_buffer, Model::Molecule & p_molecule ) { return false; }

		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
