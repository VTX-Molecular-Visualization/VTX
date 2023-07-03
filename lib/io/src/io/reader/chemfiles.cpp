#include "io/reader/chemfiles.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <util/chrono.hpp>
#include <util/color/rgba.hpp>
#include <util/exceptions.hpp>
#include <util/logger.hpp>

namespace VTX::IO::Reader
{
	std::unique_ptr<Chemfiles> Chemfiles::readFile( const FilePath & p_path )
	{
		Util::Chrono chrono = Util::Chrono();
		chrono.start();

		chemfiles::warning_callback_t callback = [ & ]( const std::string & p_log ) { _warningCallback( p_log ); };
		chemfiles::set_warning_callback( callback );

		std::unique_ptr<Chemfiles> chemfilesReader = std::make_unique<Chemfiles>( p_path );

		chrono.stop();
		VTX_INFO( "readFile : {}", chrono.elapsedTimeStr() );

		return chemfilesReader;
	}
	std::unique_ptr<Chemfiles> Chemfiles::readBuffer( const std::string & p_buffer, const FilePath & p_path )
	{
		Util::Chrono chrono = Util::Chrono();
		chrono.start();

		chemfiles::warning_callback_t callback = [ & ]( const std::string & p_log ) { _warningCallback( p_log ); };
		chemfiles::set_warning_callback( callback );

		std::unique_ptr<Chemfiles> chemfilesReader = std::make_unique<Chemfiles>( p_buffer, p_path );

		chrono.stop();
		VTX_INFO( "readBuffer : {}", chrono.elapsedTimeStr() );

		return chemfilesReader;
	}

	Chemfiles::Chemfiles( const FilePath & p_path ) :
		_path( p_path ), _trajectory( chemfiles::Trajectory( p_path.string(), 'r', _getFormat( p_path ) ) )
	{
		_readTrajectory();
	}
	Chemfiles::Chemfiles( const std::string & p_buffer, const FilePath & p_path ) :
		_path( p_path ),
		_trajectory( chemfiles::Trajectory::memory_reader( p_buffer.c_str(), p_buffer.size(), _getFormat( p_path ) ) )
	{
		_readTrajectory();
	}

	void Chemfiles::_readTrajectory()
	{
		if ( _trajectory.nsteps() == 0 )
		{
			throw IOException( "Trajectory is empty" );
		}

		VTX_INFO( "{} frames found.", _trajectory.nsteps() );

		Util::Chrono chrono;

		chrono.start();
		_preRead();
		chrono.stop();
		VTX_INFO( "_preRead: {}", chrono.elapsedTimeStr() );

		chrono.start();
		chemfiles::Frame frame;
		_read();
		chrono.stop();
		VTX_INFO( "Trajectory read in: {}", chrono.elapsedTimeStr() );

		chrono.start();
		_postRead();
		chrono.stop();
		VTX_INFO( "_postRead: {}", chrono.elapsedTimeStr() );
	}

	void Chemfiles::_preRead()
	{
		// If opening a DCD file check if a topology file is present in the same folder.
		if ( _path.extension() == "dcd" )
		{
			const std::string			   stem			  = _path.stem().string();
			const std::vector<std::string> topExtensions  = { ".xyz", ".pdb", ".mol2" };
			std::string					   foundExtension = "";
			for ( size_t ext = 0; ext < topExtensions.size(); ext++ )
			{
				std::fstream topFile;
				topFile.open( stem + topExtensions[ ext ] );
				if ( topFile.is_open() )
				{
					topFile.close();
					foundExtension = topExtensions[ ext ];
					break;
				}
			}
			if ( foundExtension != "" )
			{
				// usefull ?
				// chemfiles::Trajectory topolgy_file( stem + foundExtension, 'r' );
				_trajectory.set_topology( stem + foundExtension );
			}
		}
	}
	void Chemfiles::_read()
	{
		_currentFrame = _trajectory.read();
		_topology	  = _currentFrame.topology();
		_residues	  = &( _topology.residues() );
		_bonds		  = &( _topology.bonds() );
	}
	void Chemfiles::_postRead()
	{
		const std::vector<chemfiles::Residue> & residues = _topology.residues();

		if ( residues.size() == 0 )
		{
			// If no residue, create a fake one.
			// TODO: check file format instead of residue count?
			VTX_INFO( "No residues found" );
			chemfiles::Residue residue = chemfiles::Residue( "UNK", 0 );
			for ( size_t i = 0; i < _currentFrame.size(); ++i )
			{
				residue.add_atom( i );
			}
			_currentFrame.add_residue( residue );
		}

		if ( _currentFrame.size() != _topology.size() )
		{
			throw IOException( "Data count missmatch" );
		}

		// Check properties, same for all atoms/residues?
		if ( _currentFrame.size() > 0 )
		{
			std::string propAtom = std::to_string( _currentFrame[ 0 ].properties().size() ) + " properties in atoms:";
			for ( chemfiles::property_map::const_iterator it = _currentFrame[ 0 ].properties().begin();
				  it != _currentFrame[ 0 ].properties().end();
				  ++it )
			{
				propAtom += " " + it->first;
			}
			VTX_DEBUG( "{}", propAtom );
		}

		if ( residues.size() > 0 )
		{
			std::string propResidue
				= std::to_string( residues[ 0 ].properties().size() ) + " properties in  residues :";

			for ( chemfiles::property_map::const_iterator it = residues[ 0 ].properties().begin();
				  it != residues[ 0 ].properties().end();
				  ++it )
			{
				propResidue += " " + it->first;
			}
			VTX_DEBUG( "{}", propResidue );
		}
	}

	void Chemfiles::readNextFrame() { _currentFrame = _trajectory.read(); }

	std::vector<Vec3f> Chemfiles::getCurrentFrameAtomPosition() const
	{
		std::vector<Vec3f> res = std::vector<Vec3f>();

		const std::vector<chemfiles::Vector3D> & positions = _currentFrame.positions();

		res.resize( positions.size() );

		for ( size_t positionIdx = 0; positionIdx < positions.size(); ++positionIdx )
		{
			const chemfiles::Vector3D & position = positions[ positionIdx ];
			res[ positionIdx ]					 = { position[ 0 ], position[ 1 ], position[ 2 ] };
		}

		return res;
	}

	VTX::Core::ChemDB::Atom::SYMBOL Chemfiles::getCurrentAtomSymbol() const
	{
		return VTX::Core::ChemDB::Atom::getSymbolFromString( _currentAtom->type() );
	}

	Vec3f Chemfiles::getCurrentAtomPosition() const
	{
		const std::vector<chemfiles::Vector3D> & positions = _currentFrame.positions();
		const chemfiles::Vector3D &				 position  = positions[ _currentAtomIndex ];

		return Vec3f( position[ 0 ], position[ 1 ], position[ 2 ] );
	}

	// http://chemfiles.org/chemfiles/latest/formats.html#list-of-supported-formats
	const std::string Chemfiles::_getFormat( const FilePath & p_path )
	{
		std::string extension = p_path.extension().string();
		std::transform( extension.begin(), extension.end(), extension.begin(), tolower );
		if ( extension == ".nc" )
		{
			return "Amber NetCDF";
		}
		else if ( extension == ".cif" )
		{
			return "mmCIF"; // Workaround.
		}
		else if ( extension == ".cml" )
		{
			return "CML";
		}
		else if ( extension == ".cssr" )
		{
			return "CSSR";
		}
		else if ( extension == ".dcd" )
		{
			return "DCD";
		}
		else if ( extension == ".gro" )
		{
			return "GRO";
		}
		else if ( extension == ".lammpstrj" )
		{
			return "LAMMPS";
		}
		else if ( extension == ".mmcif" )
		{
			return "mmCIF";
		}
		else if ( extension == ".mmtf" )
		{
			return "MMTF";
		}
		else if ( extension == ".mol2" )
		{
			return "MOL2";
		}
		else if ( extension == ".molden" )
		{
			return "Molden";
		}
		else if ( extension == ".pdb" )
		{
			return "PDB";
		}
		else if ( extension == ".sdf" )
		{
			return "SDF";
		}
		else if ( extension == ".smi" )
		{
			return "SMI";
		}
		else if ( extension == ".arc" )
		{
			return "Tinker";
		}
		else if ( extension == ".tng" )
		{
			return "TNG";
		}
		else if ( extension == ".trj" )
		{
			return "TRJ";
		}
		else if ( extension == ".trr" )
		{
			return "TRR";
		}
		else if ( extension == ".xtc" )
		{
			return "XTC";
		}
		else if ( extension == ".xyz" )
		{
			return "XYZ";
		}
		else
		{
			assert( false );
			return "Unknown";
		}
	}

	void Chemfiles::_warningCallback( const std::string & p_log ) {}

} // namespace VTX::IO::Reader
