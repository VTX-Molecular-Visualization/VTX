#include "io/reader/chemfiles.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <util/chrono.hpp>
#include <util/color/rgba.hpp>
#include <util/exceptions.hpp>
#include <util/logger.hpp>
#include <util/string.hpp>

#pragma warning( push, 0 )
#include <chemfiles.hpp>
#pragma warning( pop )

namespace VTX::IO::Reader
{
	struct Chemfiles::ReadingData
	{
		explicit ReadingData( const FilePath & p_path, const std::string & p_format ) :
			_trajectory( chemfiles::Trajectory( p_path.string(), 'r', p_format ) )
		{
		}

		explicit ReadingData( const std::string & p_buffer, const FilePath & p_path, const std::string & p_format ) :
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
	struct Chemfiles::ResidueIt::InternalResidueIt
	{
		InternalResidueIt()							   = default;
		InternalResidueIt( const InternalResidueIt & ) = default;

		InternalResidueIt( chemfiles::Residue::const_iterator p_it ) : it( p_it ) {}
		~InternalResidueIt() = default;

		chemfiles::Residue::const_iterator it;
	};

	Chemfiles::ResidueIt::ResidueIt() : _internalIterator( nullptr ) {};
	Chemfiles::ResidueIt::ResidueIt( const ResidueIt & p_source ) :
		_internalIterator( std::make_unique<InternalResidueIt>( *p_source._internalIterator ) ) {};

	Chemfiles::ResidueIt::ResidueIt( InternalResidueIt & p_internalIt ) :
		_internalIterator( std::make_unique<InternalResidueIt>( p_internalIt ) )
	{
	}

	Chemfiles::ResidueIt::~ResidueIt() = default;

	atom_index_t Chemfiles::ResidueIt::operator*() const { return atom_index_t( *( _internalIterator->it ) ); }
	atom_index_t Chemfiles::ResidueIt::operator->() const { return atom_index_t( *( _internalIterator->it ) ); }

	// Prefix increment
	Chemfiles::ResidueIt & Chemfiles::ResidueIt::operator++()
	{
		++_internalIterator->it;
		return *this;
	}

	// Postfix increment
	Chemfiles::ResidueIt Chemfiles::ResidueIt::operator++( int )
	{
		Chemfiles::ResidueIt oldIt = *this;
		_internalIterator->it++;
		return oldIt;
	}

	bool operator==( const Chemfiles::ResidueIt & p_lhs, const Chemfiles::ResidueIt & p_rhs )
	{
		return p_lhs._internalIterator->it == p_rhs._internalIterator->it;
	}
	bool operator!=( const Chemfiles::ResidueIt & p_lhs, const Chemfiles::ResidueIt & p_rhs )
	{
		return p_lhs._internalIterator->it != p_rhs._internalIterator->it;
	}

	std::unique_ptr<Chemfiles> Chemfiles::readFile( const FilePath & p_path )
	{
		Util::Chrono chrono = Util::Chrono();
		chrono.start();

		chemfiles::warning_callback_t callback = [ & ]( const std::string & p_log ) { _warningCallback( p_log ); };
		chemfiles::set_warning_callback( callback );

		std::unique_ptr<Chemfiles> chemfilesReader = std::make_unique<Chemfiles>( p_path );

		chrono.stop();
		VTX_INFO( "readFile : {}", Util::String::durationToStr( chrono.elapsedTime() ) );

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
		VTX_INFO( "readBuffer : {}", Util::String::durationToStr( chrono.elapsedTime() ) );

		return chemfilesReader;
	}

	Chemfiles::Chemfiles( const FilePath & p_path ) :
		_readingData( std::make_unique<ReadingData>( p_path, _getFormat( p_path ) ) )
	{
		_readTrajectory();
	}
	Chemfiles::Chemfiles( const std::string & p_buffer, const FilePath & p_path ) :
		_path( p_path ), _readingData( std::make_unique<ReadingData>( p_buffer, p_path, _getFormat( p_path ) ) )
	{
		_readTrajectory();
	}
	Chemfiles::~Chemfiles() = default;

	void Chemfiles::_readTrajectory()
	{
		if ( _readingData->_trajectory.nsteps() == 0 )
		{
			throw IOException( "Trajectory is empty" );
		}

		VTX_INFO( "{} frames found.", _readingData->_trajectory.nsteps() );

		Util::Chrono chrono;

		chrono.start();
		_preRead();
		chrono.stop();
		VTX_INFO( "_preRead: {}", Util::String::durationToStr( chrono.elapsedTime() ) );

		chrono.start();
		chemfiles::Frame frame;
		_read();
		chrono.stop();
		VTX_INFO( "Trajectory read in: {}", Util::String::durationToStr( chrono.elapsedTime() ) );

		chrono.start();
		_postRead();
		chrono.stop();
		VTX_INFO( "_postRead: {}", Util::String::durationToStr( chrono.elapsedTime() ) );
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
				_readingData->_trajectory.set_topology( stem + foundExtension );
			}
		}
	}
	void Chemfiles::_read() { _readingData->read(); }
	void Chemfiles::_postRead()
	{
		const std::vector<chemfiles::Residue> & residues = *( _readingData->_residues );

		if ( residues.size() == 0 )
		{
			// If no residue, create a fake one.
			// TODO: check file format instead of residue count?
			VTX_INFO( "No residues found" );
			chemfiles::Residue residue = chemfiles::Residue( "UNK", 0 );
			for ( size_t i = 0; i < _readingData->_currentFrame.size(); ++i )
			{
				residue.add_atom( i );
			}
			_readingData->_currentFrame.add_residue( residue );
		}

		if ( _readingData->_currentFrame.size() != _readingData->_topology.size() )
		{
			throw IOException( "Data count missmatch" );
		}

		// Check properties, same for all atoms/atoms?
		if ( _readingData->_currentFrame.size() > 0 )
		{
			if ( _readingData->_currentFrame[ 0 ].properties() )
			{
				std::string propAtom
					= std::to_string( _readingData->_currentFrame[ 0 ].properties()->size() ) + " properties in atoms:";
				for ( chemfiles::property_map::const_iterator it
					  = _readingData->_currentFrame[ 0 ].properties()->begin();
					  it != _readingData->_currentFrame[ 0 ].properties()->end();
					  ++it )
				{
					propAtom += " " + it->first;
				}
				VTX_DEBUG( "{}", propAtom );
			}
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

	void Chemfiles::readNextFrame() { _readingData->readNextFrame(); }

	// Trajectory //////////////////////////////////
	size_t Chemfiles::getFrameCount() const { return _readingData->_trajectory.nsteps(); }
	size_t Chemfiles::getResidueCount() const { return _readingData->_residues->size(); }
	size_t Chemfiles::getAtomCount() const { return _readingData->_currentFrame.size(); }
	size_t Chemfiles::getBondCount() const { return _readingData->_currentFrame.topology().bonds().size(); }

	// Frame //////////////////////////////////
	const std::string Chemfiles::getFrameName() const
	{
		return _readingData->_currentFrame.get( "name" ) ? _readingData->_currentFrame.get( "name" )->as_string() : "";
	}
	const std::string Chemfiles::getPdbIdCode() const
	{
		return _readingData->_currentFrame.get( "pdb_idcode" )
				   ? _readingData->_currentFrame.get( "pdb_idcode" )->as_string()
				   : "";
	}

	// Residues //////////////////////////////////
	void Chemfiles::setCurrentResidue( const size_t p_residueIndex )
	{
		_readingData->_currentResidue = &( ( *_readingData->_residues )[ p_residueIndex ] );
	}
	const std::string Chemfiles::getCurrentResidueStringProperty(
		const std::string & p_property,
		const std::string & p_defaultValue
	) const
	{
		// Seems to be faster that way than using value_or function for string&.
		const std::experimental::optional<const ::chemfiles::Property &> & optionalProperty
			= _readingData->_currentResidue->properties().get( p_property );
		return optionalProperty ? optionalProperty.value().as_string() : p_defaultValue;
	}
	const double Chemfiles::getCurrentResidueDoubleProperty(
		const std::string & p_property,
		const double		p_defaultValue
	) const
	{
		return _readingData->_currentResidue->properties().get( p_property ).value_or( p_defaultValue ).as_double();
	}
	const bool Chemfiles::getCurrentResidueBoolProperty( const std::string & p_property, const bool p_defaultValue )
		const
	{
		return _readingData->_currentResidue->properties().get( p_property ).value_or( p_defaultValue ).as_bool();
	}

	const std::string & Chemfiles::getCurrentResidueName() const { return _readingData->_currentResidue->name(); }
	const size_t		Chemfiles::getCurrentResidueId() const
	{
		return size_t( _readingData->_currentResidue->id().value_or( INVALID_INDEX ) );
	}
	const atom_index_t Chemfiles::getCurrentResidueFirstAtomIndex() const
	{
		return atom_index_t( *( _readingData->_currentResidue->begin() ) );
	}
	const atom_index_t Chemfiles::getCurrentResidueAtomCount() const
	{
		return atom_index_t( _readingData->_currentResidue->size() );
	}

	Chemfiles::ResidueIt Chemfiles::getCurrentResidueAtomIteratorBegin() const
	{
		ResidueIt::InternalResidueIt itBegin = ResidueIt::InternalResidueIt( _readingData->_currentResidue->cbegin() );
		return Chemfiles::ResidueIt( itBegin );
	}
	Chemfiles::ResidueIt Chemfiles::getCurrentResidueAtomIteratorEnd() const
	{
		ResidueIt::InternalResidueIt itEnd = ResidueIt::InternalResidueIt( _readingData->_currentResidue->cend() );
		return Chemfiles::ResidueIt( itEnd );
	}

	// Atom //////////////////////////////////////
	void Chemfiles::setCurrentAtom( const size_t p_index )
	{
		_readingData->_currentAtom		= &( _readingData->_currentFrame[ p_index ] );
		_readingData->_currentAtomIndex = p_index;
	}
	const std::string Chemfiles::getCurrentAtomStringProperty(
		const std::string & p_property,
		const std::string & p_defaultValue
	) const
	{
		// Seems to be faster that way than using value_or function for string&.

		if ( !_readingData->_currentAtom->properties() )
			return p_defaultValue;

		const std::experimental::optional<const ::chemfiles::Property &> & optionalProperty
			= _readingData->_currentAtom->properties()->get( p_property );
		return optionalProperty ? optionalProperty.value().as_string() : p_defaultValue;
	}
	const double Chemfiles::getCurrentAtomDoubleProperty( const std::string & p_property, const double p_defaultValue )
		const
	{
		if ( !_readingData->_currentAtom->properties() )
			return p_defaultValue;

		return _readingData->_currentAtom->properties()->get( p_property ).value_or( p_defaultValue ).as_double();
	}
	const bool Chemfiles::getCurrentAtomBoolProperty( const std::string & p_property, const bool p_defaultValue ) const
	{
		if ( !_readingData->_currentAtom->properties() )
			return p_defaultValue;

		return _readingData->_currentAtom->properties()->get( p_property ).value_or( p_defaultValue ).as_bool();
	}

	const std::string & Chemfiles::getCurrentAtomName() const { return _readingData->_currentAtom->name(); }
	std::vector<Vec3f>	Chemfiles::getCurrentFrameAtomPosition() const
	{
		std::vector<Vec3f> res = std::vector<Vec3f>();

		const chemfiles::span<chemfiles::Vector3D> & positions = _readingData->_currentFrame.positions();

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
		return VTX::Core::ChemDB::Atom::getSymbolFromString( _readingData->_currentAtom->type() );
	}
	int Chemfiles::getCurrentAtomType() const { return int( getCurrentAtomDoubleProperty( "atom_type", -1. ) ); }

	Vec3f Chemfiles::getCurrentAtomPosition() const
	{
		const chemfiles::span<chemfiles::Vector3D> & positions = _readingData->_currentFrame.positions();
		const chemfiles::Vector3D &					 position  = positions[ _readingData->_currentAtomIndex ];

		return Vec3f( position[ 0 ], position[ 1 ], position[ 2 ] );
	}

	// Bonds /////////////////////////////////////////
	void Chemfiles::setCurrentBond( const size_t p_bondIndex )
	{
		_readingData->_currentBond		= &( ( *_readingData->_bonds )[ p_bondIndex ] );
		_readingData->_currentBondIndex = p_bondIndex;
	}

	atom_index_t Chemfiles::getCurrentBondFirstAtomIndex() const
	{
		return atom_index_t( ( *_readingData->_currentBond )[ 0 ] );
	}
	atom_index_t Chemfiles::getCurrentBondSecondAtomIndex() const
	{
		return atom_index_t( ( *_readingData->_currentBond )[ 1 ] );
	}

	const VTX::Core::ChemDB::Bond::ORDER Chemfiles::getCurrentBondOrder() const
	{
		const chemfiles::Bond::BondOrder & bondOrder
			= _readingData->_topology.bond_orders()[ _readingData->_currentBondIndex ];
		return VTX::Core::ChemDB::Bond::ORDER( int( bondOrder ) );
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
			throw IOException( "Unknown file format: {}", extension );
		}
	}

	void Chemfiles::_warningCallback( const std::string & p_log ) {}

} // namespace VTX::IO::Reader
