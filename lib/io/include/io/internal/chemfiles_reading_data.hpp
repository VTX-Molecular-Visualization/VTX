#ifndef __VTX_IO_INTERNAL_CHEMFILES_READING_DATA__
#define __VTX_IO_INTERNAL_CHEMFILES_READING_DATA__

#include <string>
#include <util/types.hpp>
#include <vector>
#pragma warning( push, 0 )
#include <chemfiles.hpp>
#pragma warning( pop )

namespace VTX::IO::Internal
{
	struct ChemfilesReadingData
	{
		ChemfilesReadingData( const FilePath & p_path, const std::string & p_format ) :
			_trajectory( chemfiles::Trajectory( p_path.string(), 'r', p_format ) )
		{
		}

		ChemfilesReadingData( const std::string & p_buffer, const FilePath & p_path, const std::string & p_format ) :
			_trajectory( chemfiles::Trajectory::memory_reader( p_buffer.c_str(), p_buffer.size(), p_format ) )
		{
		}

		chemfiles::Trajectory					_trajectory;
		chemfiles::Frame						_currentFrame = chemfiles::Frame();
		chemfiles::Topology						_topology	  = chemfiles::Topology();
		const std::vector<chemfiles::Residue> * _residues	  = nullptr;
		const std::vector<chemfiles::Bond> *	_bonds		  = nullptr;

		const chemfiles::Residue * _currentResidue	 = nullptr;
		const chemfiles::Atom *	   _currentAtom		 = nullptr;
		size_t					   _currentAtomIndex = -1;
		const chemfiles::Bond *	   _currentBond		 = nullptr;
		size_t					   _currentBondIndex = -1;

		void read()
		{
			_currentFrame = _trajectory.read();
			_topology	  = _currentFrame.topology();
			_residues	  = &( _topology.residues() );
			_bonds		  = &( _topology.bonds() );
		}

		void readNextFrame() { _currentFrame = _trajectory.read(); }
	};
} // namespace VTX::IO::Internal
#endif
