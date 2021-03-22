#include "writer_chemfiles.hpp"
#include "model/atom.hpp"
#include "model/bond.hpp"
#include "model/residue.hpp"
#include "tool/chrono.hpp"

namespace VTX
{
	namespace IO
	{
		namespace Writer
		{
			void ChemfilesWriter::writeFile( const FilePath & p_path, const Model::Molecule & p_molecule )
			{
				chemfiles::Trajectory trajectory = chemfiles::Trajectory( p_path.string(), 'a' );

				writeTrajectory( trajectory, p_path, p_molecule );
			}

			void ChemfilesWriter::fillTrajectoryFrames( chemfiles::Trajectory & p_trajectory,
														Model::Molecule &		p_molecule ) const
			{
				// Fill other frames.
				Tool::Chrono timeReadingFrames;
				timeReadingFrames.start();
				int startingFrame = 1;
				for ( uint frameIdx = 1; frameIdx < p_trajectory.nsteps(); ++frameIdx )
				{
					Model::Molecule::AtomPositionsFrame & moleculeFrame = p_molecule.getAtomPositionFrame( frameIdx );
					chemfiles::Frame					  frame			= p_trajectory.read_step( frameIdx );
					const chemfiles::span<chemfiles::Vector3D> & positions = frame.positions();
					for ( uint positionIdx = 0; positionIdx < positions.size(); ++positionIdx )
					{
						const chemfiles::Vector3D & position = positions[ positionIdx ];
						moleculeFrame[ positionIdx ]		 = { position[ 0 ], position[ 1 ], position[ 2 ] };
					}
#ifdef _DEBUG
					if ( frameIdx % 100 == 0 )
					{
						VTX_DEBUG( "Frames from " + std::to_string( startingFrame ) + " to "
								   + std::to_string( frameIdx )
								   + " read in: " + std::to_string( timeReadingFrames.intervalTime() ) + "s" );
						startingFrame = frameIdx;
					}
#endif // DEBUG
				}
				timeReadingFrames.stop();
				VTX_INFO( "Frames read in: " + std::to_string( timeReadingFrames.elapsedTime() ) + "s" );
			}

			void ChemfilesWriter::writeTrajectory( chemfiles::Trajectory & p_trajectory,
												   const FilePath &		   p_path,
												   const Model::Molecule & p_molecule ) const
			{
				auto frame = chemfiles::Frame();
				frame.reserve( p_molecule.getAtomCount() );
				frame.set( "name", p_molecule.getName() );
				// add residues
				for ( uint residue = 0; residue < p_molecule.getResidueCount(); residue++ )
				{
					const VTX::Model::Residue * res				= p_molecule.getResidue( residue );
					uint						firstResAtomIdx = res->getIndexFirstAtom();
					chemfiles::Residue			chemRes			= chemfiles::Residue( res->getSymbolStr(), ++residue );
					for ( firstResAtomIdx; firstResAtomIdx < res->getAtomCount(); ++firstResAtomIdx )
					{
						const VTX::Model::Atom * atm	 = p_molecule.getAtom( firstResAtomIdx );
						VTX::Vec3f				 atmPos	 = p_molecule.getAtomPositionFrame( 0 )[ firstResAtomIdx ];
						chemfiles::Vector3D		 atomPos = chemfiles::Vector3D( atmPos[ 0 ], atmPos[ 1 ], atmPos[ 2 ] );
						frame.add_atom( chemfiles::Atom( atm->getSymbolStr() ), atomPos );
						chemRes.add_atom( firstResAtomIdx );
					}
					frame.add_residue( chemRes );
				}
				// add bonds
				for ( VTX::uint bond = 0; bond < p_molecule.getBondCount(); bond++ )
				{
					const VTX::Model::Bond * bnd = p_molecule.getBond( bond );
					frame.add_bond( bnd->getIndexFirstAtom(), bnd->getIndexSecondAtom() );
				}
				p_trajectory.write( frame );
			}
		} // namespace Writer
	}	  // namespace IO
} // namespace VTX
