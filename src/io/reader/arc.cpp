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
				chemfiles::Frame frame = trajectory.read();

				chemfiles::span<chemfiles::Vector3D> positions = frame.positions();

				p_molecule.addAtomPositionFrame();
				Model::Molecule::AtomPositionsFrame & moleculeFrame = p_molecule.getAtomPositionFrame( 0 );

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

				// TODO: Loop over frames.

				// Test only the first frame
				VTX_INFO( std::to_string( frame.size() ) + " atoms in the frame" );
				for ( uint i = 0; i < positions.size(); ++i )
				{
					Model::Atom & atom = p_molecule.getAtom( i );
					atom.setMoleculePtr( &p_molecule );
					atom.setChainPtr( &chain );
					atom.setResiduePtr( &residue );
					atom.setColor( Util::Color::randomPastelColor() );

					chemfiles::Vector3D & position = positions[ i ];
					moleculeFrame.emplace_back( Vec3f( position[ 0 ], position[ 1 ], position[ 2 ] ) );
					p_molecule.addAtomRadius( 1.20f );

					const Vec3f & atomPosition = moleculeFrame[ i ];
					const float	  atomRadius   = 1.20f;
					// VTX_DEBUG( glm::to_string( atomPosition ) );
					p_molecule.extendAABB( atomPosition, atomRadius );
				}

				return true;
			}

			bool ARC::readBuffer( const std::string & p_buffer, Model::Molecule & p_molecule ) { return false; }

		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
