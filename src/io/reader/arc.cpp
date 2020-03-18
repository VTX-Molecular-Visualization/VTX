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

				// TODO: Loop over frames.

				// Test only the first frame
				VTX_INFO( std::to_string( frame.size() ) + " atoms in the frame" );
				for ( uint i = 0; i < positions.size(); ++i )
				{
					chemfiles::Vector3D & position = positions[ i ];
					moleculeFrame.emplace_back( Vec3f( position[ 0 ], position[ 1 ], position[ 2 ] ) );
					p_molecule.addAtomRadius( 1.20f );
					p_molecule.addAtomColor( Util::Color::randomPastelColor() );

					const Vec3f & atomPosition = moleculeFrame[ i ];
					const float	  atomRadius   = 1.20f;

					p_molecule.extendAABB( atomPosition, atomRadius );
				}

				return true;
			}

			bool ARC::readBuffer( const std::string & p_buffer, Model::Molecule & p_molecule ) { return false; }

		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
