#include "io/writer/chemfiles.hpp"
#include <list>
#include <map>
#include <unordered_map>
#include <util/exceptions.hpp>
#include <util/string.hpp>
//
#include <chemfiles.h>
#include <chemfiles/Frame.hpp>
#include <chemfiles/Trajectory.hpp>

namespace std
{

	template<>
	class hash<VTX::IO::Writer::AtomId>
	{
	  public:
		size_t operator()( const VTX::IO::Writer::AtomId & p_ ) const noexcept { return p_.value; }
	};
} // namespace std

namespace VTX::IO::Writer
{
	using PropertyCollection = std::unordered_map<std::string, PropertyValue>;

	// All struct starting with an underscore are private to this .obj

	/**
	 * @brief data struct used by the Atom to share information with a part of its client. Only callers that knows this
	 * definition can actually use this.
	 */
	struct AtomInfo
	{
		AtomImpl * dataPtr;
	};

	struct AtomImpl
	{
		AtomId		externalId;
		std::string name;
		std::string symbol;
	};

	using AtomCollection			= std::map<AtomId, AtomImpl>;
	using AtomCoordinatesCollection = std::unordered_map<const AtomImpl *, AtomCoordinates>;
	using AtomPtrCollection			= std::list<const AtomImpl *>;

	struct ResidueImpl
	{
		int				   resid {};
		std::string		   symbol;
		PropertyCollection properties;
		AtomPtrCollection  atoms;
	};

	using ResidueCollection = std::list<ResidueImpl>;

	struct ChainImpl
	{
		std::string name;
		std::string id;
	};

	using ChainCollection = std::list<ChainImpl>; // Used for constant time insertion and memory efficiency

	struct FrameImpl
	{
		AtomCoordinatesCollection coordinates;
	};

	using FrameCollection = std::list<FrameImpl>; // Used for constant time insertion and memory efficiency

	namespace
	{
		struct Bond
		{
			AtomId		 a1;
			AtomId		 a2;
			E_BOND_ORDER bondOrder;
		};
	} // namespace

	using BondCollection = std::list<Bond>; // Used for constant time insertion and memory efficiency

	struct SystemImpl
	{
		PropertyCollection properties;
		AtomCollection	   atoms;
		BondCollection	   bonds;
		ChainCollection	   chains;
		ResidueCollection  residues;
		FrameCollection	   frames;
	};

	namespace
	{
		void figureFormat( const FilePath & dest, E_FILE_FORMATS & format )
		{
			if ( not dest.has_extension() )
			{
				return;
			}
			std::string extension = dest.extension().string();
			extension			  = VTX::Util::String::toUpper( extension );

			if ( extension == ".MMCIF" )
			{
				format = E_FILE_FORMATS::mmcif;
			}
			else if ( extension == ".PDB" )
			{
				format = E_FILE_FORMATS::pdb;
			}
			else if ( extension == ".MMTF" )
			{
				format = E_FILE_FORMATS::mmtf;
			}
			else if ( extension == ".MOLDEN" )
			{
				format = E_FILE_FORMATS::molden;
			}
			else if ( extension == ".CIF" )
			{
				format = E_FILE_FORMATS::cif;
			}
			else if ( extension == ".GRO" )
			{
				format = E_FILE_FORMATS::gro;
			}
			else if ( extension == ".MOL2" )
			{
				format = E_FILE_FORMATS::mol2;
			}
			else if ( extension == ".SDf" )
			{
				format = E_FILE_FORMATS::sdf;
			}
			else if ( extension == ".SMI" )
			{
				format = E_FILE_FORMATS::smi;
			}
			else if ( extension == ".XYZ" )
			{
				format = E_FILE_FORMATS::xyz;
			}
			else if ( extension == ".CML" )
			{
				format = E_FILE_FORMATS::cml;
			}
			else if ( extension == ".CSSR" )
			{
				format = E_FILE_FORMATS::cssr;
			}
			else if ( extension == ".NC" )
			{
				format = E_FILE_FORMATS::nc;
			}
			else if ( extension == ".DCD" )
			{
				format = E_FILE_FORMATS::nc;
			}
			else if ( extension == ".LAMMPSTRJ" )
			{
				format = E_FILE_FORMATS::lammpstrj;
			}
			else if ( extension == ".ARC" )
			{
				format = E_FILE_FORMATS::arc;
			}
			else if ( extension == ".TRR" )
			{
				format = E_FILE_FORMATS::trr;
			}
			else if ( extension == ".XTC" )
			{
				format = E_FILE_FORMATS::xtc;
			}
			else if ( extension == ".TNG" )
			{
				format = E_FILE_FORMATS::tng;
			}
			else if ( extension == ".TRJ" )
			{
				format = E_FILE_FORMATS::trj;
			}
		}

		const char * string( const E_FILE_FORMATS & format ) noexcept
		{
			switch ( format )
			{
			case E_FILE_FORMATS::pdb: return "PDB";
			case E_FILE_FORMATS::mmcif: return "mmCIF";
			case E_FILE_FORMATS::mmtf: return "MMTF";
			case E_FILE_FORMATS::molden: return "Molden";
			case E_FILE_FORMATS::cif: return "CIF";
			case E_FILE_FORMATS::gro: return "GRO";
			case E_FILE_FORMATS::mol2: return "MOL2";
			case E_FILE_FORMATS::sdf: return "SDF";
			case E_FILE_FORMATS::smi: return "SMI";
			case E_FILE_FORMATS::xyz: return "XYZ";
			case E_FILE_FORMATS::cml: return "CML";
			case E_FILE_FORMATS::cssr: return "CSSR";
			case E_FILE_FORMATS::nc: return "Amber NetCDF";
			case E_FILE_FORMATS::dcd: return "DCD";
			case E_FILE_FORMATS::lammpstrj: return "LAMMPS";
			case E_FILE_FORMATS::arc: return "Tinker";
			case E_FILE_FORMATS::trr: return "TRR";
			case E_FILE_FORMATS::xtc: return "XTC";
			case E_FILE_FORMATS::tng: return "TNG";
			case E_FILE_FORMATS::trj: return "TRJ";
			default: break;
			}
			return "";
		}

		template<typename T>
		inline void setProperty( T & cf, const std::string & key, const std::string & value )
		{ cf.set( key, value ); }

		template<typename T>
		inline void setProperty( T & cf, const std::string & key, const double & value )
		{ cf.set( key, value ); }

		template<typename T>
		inline void setProperty( T & cf, const std::string & key, const bool & value )
		{ cf.set( key, value ); }

		template<typename T>
		void transferProperties( T & cf, const PropertyCollection & props )
		{
			for ( const auto & prop : props )
			{
				std::visit( [ & ]( const auto & v ) { setProperty<T>( cf, prop.first, v ); }, prop.second );
			}
		}

		inline void convert( const AtomCoordinates & in, ::chemfiles::Vector3D & out ) noexcept
		{ out = ::chemfiles::Vector3D( in.x, in.y, in.z ); }

		inline void convert( const E_BOND_ORDER & in, ::chemfiles::Bond::BondOrder & out ) noexcept
		{
			switch ( in )
			{
			case E_BOND_ORDER::unknown: out = ::chemfiles::Bond::BondOrder::UNKNOWN; break;
			case E_BOND_ORDER::single: out = ::chemfiles::Bond::BondOrder::SINGLE; break;
			case E_BOND_ORDER::doubl: out = ::chemfiles::Bond::BondOrder::DOUBLE; break;
			case E_BOND_ORDER::triple: out = ::chemfiles::Bond::BondOrder::TRIPLE; break;
			case E_BOND_ORDER::quadruple: out = ::chemfiles::Bond::BondOrder::QUADRUPLE; break;
			case E_BOND_ORDER::quintuple: out = ::chemfiles::Bond::BondOrder::QUINTUPLET; break;
			case E_BOND_ORDER::down: out = ::chemfiles::Bond::BondOrder::DOWN; break;
			case E_BOND_ORDER::up: out = ::chemfiles::Bond::BondOrder::UP; break;
			case E_BOND_ORDER::dative_L: out = ::chemfiles::Bond::BondOrder::DATIVE_L; break;
			case E_BOND_ORDER::dative_R: out = ::chemfiles::Bond::BondOrder::DATIVE_R; break;
			case E_BOND_ORDER::amide: out = ::chemfiles::Bond::BondOrder::AMIDE; break;
			case E_BOND_ORDER::aromatic: out = ::chemfiles::Bond::BondOrder::AROMATIC; break;
			default: out = ::chemfiles ::Bond ::BondOrder ::UNKNOWN; break;
			}
		}

		void writeFile( const FilePath & dest, SystemImpl & system )
		{
			// TODO I disabled the format things as chemfiles has a format guessing adequat enough
			::chemfiles::Trajectory cf_traj( dest.string(), 'w' /*, string( format )*/ );

			std::unordered_map<AtomId, size_t> atomIdCorrespondaceTable; // We need a correspondance table between the
																		 // ID that chemfiles accepts and the ID we
			bool firstFrameWritten = true; // We need to keep track if the frame loop we're into is the first one as we
										   // need to fill the corresponande table only once and for all.

			for ( auto & it_frame : system.frames )
			{
				size_t atomResidueCounter
					= 0; // We need to count the atoms in the order they are presented when iterating through
						 // residues. The counter is used in chemfiles to assign an atom to a residue after
						 // having it assign to a frame. Yes, the design is questionnable.
				::chemfiles::Frame cf_frame;
				cf_frame.reserve( system.atoms.size() );
				transferProperties( cf_frame, system.properties );

				for ( auto & it_residue : system.residues )
				{
					::chemfiles::Residue cf_residue( it_residue.symbol, it_residue.resid );
					transferProperties( cf_residue, it_residue.properties );

					for ( auto & it_atom : it_residue.atoms )
					{
						const AtomCoordinates coords = it_frame.coordinates.at( it_atom );
						::chemfiles::Vector3D cf_coords;
						convert( coords, cf_coords );
						cf_frame.add_atom( ::chemfiles::Atom( it_atom->name, it_atom->symbol ), cf_coords );
						if ( firstFrameWritten )
						{
							if ( not atomIdCorrespondaceTable.contains( it_atom->externalId ) )
							{
								atomIdCorrespondaceTable.insert( { it_atom->externalId, atomResidueCounter } );
							}
						}
						cf_residue.add_atom( atomResidueCounter++ );
					}
					cf_frame.add_residue( cf_residue );
				}

				for ( auto & it_bond : system.bonds )
				{
					::chemfiles::Bond::BondOrder cf_bondOrder = ::chemfiles::Bond::BondOrder::UNKNOWN;
					convert( it_bond.bondOrder, cf_bondOrder );
					cf_frame.add_bond( atomIdCorrespondaceTable.at( it_bond.a1 ),
									   atomIdCorrespondaceTable.at( it_bond.a2 ),
									   cf_bondOrder );
				}

				cf_traj.write( cf_frame );
				firstFrameWritten = false;
			}
			cf_traj.close();
		}

	} // namespace

	class ChemfilesTrajectory::Impl
	{
		FilePath	   _dest;
		E_FILE_FORMATS _format = E_FILE_FORMATS::none;
		SystemImpl		   _system;

	  public:
		void setWriteDestination( FilePath && p_ ) noexcept { _dest = std::move( p_ ); }

		void setWriteFormat( const E_FILE_FORMATS p_ ) noexcept { _format = p_; }

		System system() noexcept { return _system; }

		Impl()							   = default;
		Impl( const Impl & )			   = delete;
		Impl( Impl && )				   = delete;
		Impl & operator=( const Impl & ) = delete;
		Impl & operator=( Impl && )	   = delete;

		void close()
		{
			if ( _system.frames.empty() )
			{
				return;
			}
			if ( _dest.empty() )
			{
				return;
			}
			// TODO I disabled the format things as chemfiles has a format guessing adequat enough
			// if ( _format == E_FILE_FORMATS::none )
			//	figureFormat( _dest, _format );
			// if ( _format == E_FILE_FORMATS::none )
			//	return;

			writeFile( _dest, _system );
			_dest.clear();
		}
	};

	// Atom
	Atom::Atom( AtomImpl & p_ ) : _data( &p_ ) {}

	void Atom::get( AtomInfo & p_out ) const noexcept { p_out = AtomInfo { .dataPtr = _data }; }

	void Atom::setName( std::string name ) noexcept
	{
		if ( _data == nullptr )
		{
			return;
		}
		_data->name = std::move( name );
	}

	void Atom::setSymbol( std::string symbol ) noexcept
	{
		if ( _data == nullptr )
		{
			return;
		}
		_data->symbol = std::move( symbol );
	}

	// Residue
	Residue::Residue( ResidueImpl & p_ ) : _data( &p_ ) {}

	void Residue::set( const Property & p_ )
	{
		if ( _data == nullptr )
		{
			return;
		}

		if ( _data->properties.contains( p_.key ) )
		{
			_data->properties.at( p_.key ) = p_.value;
		}
		else
		{
			_data->properties.emplace( std::make_pair( p_.key, p_.value ) );
		}
	}

	void Residue::setResId( int p_ ) noexcept
	{
		if ( _data == nullptr )
		{
			return;
		}

		_data->resid = p_;
	}

	void Residue::setSymbol( std::string p_symbol ) noexcept
	{
		if ( _data == nullptr )
		{
			return;
		}

		_data->symbol = std::move( p_symbol );
	}

	void Residue::add( Atom & p_ )
	{
		if ( _data == nullptr )
		{
			return;
		}

		AtomInfo atomInfo {};
		p_.get( atomInfo );

		_data->atoms.emplace_back( atomInfo.dataPtr );
	}

	// Chain
	Chain::Chain( ChainImpl & p_ ) : _data( &p_ ) {}

	void Chain::setName( std::string p_ ) noexcept
	{
		if ( _data == nullptr )
		{
			return;
		}
		_data->name = std::move( p_ );
	}

	void Chain::setId( std::string p_ ) noexcept
	{
		if ( _data == nullptr )
		{
			return;
		}
		_data->id = std::move( p_ );
	}

	void Chain::add( Residue & p_res )
	{
		if ( _data == nullptr )
		{
			return;
		}

		p_res.set( { "chainname", _data->name } );
		p_res.set( { "chainid", _data->id } );
	}

	// Frame
	Frame::Frame( FrameImpl & p_ ) : _data( &p_ ) {}

	void Frame::set( const Atom & p_atom, AtomCoordinates p_atomCoordinates )
	{
		if ( _data == nullptr )
		{
			return;
		}

		AtomInfo atomInfo {};
		p_atom.get( atomInfo );
		if ( atomInfo.dataPtr == nullptr )
		{
			return;
		}

		if ( not _data->coordinates.contains( atomInfo.dataPtr ) )
		{
			_data->coordinates.emplace( std::make_pair( atomInfo.dataPtr, p_atomCoordinates ) );
		}
		else
		{
			_data->coordinates.at( atomInfo.dataPtr ) = p_atomCoordinates;
		}
	}

	// System
	System::System( SystemImpl & p_ ) : _data( &p_ ) {}

	void System::set( Property p_ ) const
	{
		if ( _data == nullptr )
		{
			return;
		}
		if ( _data->properties.contains( p_.key ) )
		{
			_data->properties.at( p_.key ) = std::move( p_.value );
		}
		else
		{
			_data->properties.emplace( std::make_pair( std::move( p_.key ), std::move( p_.value ) ) );
		}
	}

	Atom System::newAtom( const AtomId p_atomId ) const
	{
		if ( _data == nullptr )
		{
			return {};
		}
		if ( not _data->atoms.contains( p_atomId ) )
		{
			_data->atoms.emplace( std::make_pair( p_atomId, AtomImpl { .externalId = p_atomId } ) );
		}

		return Atom( _data->atoms.at( p_atomId ) );
	}

	namespace
	{
		void vtxBondThrow( const AtomId & atom )
		{ throw VTXException( "Couldn't create bond because atom <{}> wasn't declared", atom.value ); }
	} // namespace

	void System::bind( const AtomId & p_l, const AtomId & p_r, const E_BOND_ORDER p_bondOrder ) const
	{
		if ( _data == nullptr )
		{
			return;
		}

		// First we check if the provided AtomId point toward an actual atom.
		if ( not _data->atoms.contains( p_l ) )
		{
			vtxBondThrow( p_l );
		}
		if ( not _data->atoms.contains( p_r ) )
		{
			vtxBondThrow( p_r );
		}

		_data->bonds.emplace_back( Bond { .a1 = p_l, .a2 = p_r, .bondOrder = p_bondOrder } );
	}

	Chain System::newChain() const
	{
		if ( _data == nullptr )
		{
			return {};
		}
		_data->chains.emplace_back();
		return Chain( _data->chains.back() );
	}

	Residue System::newResidue() const
	{
		if ( _data == nullptr )
		{
			return {};
		}
		_data->residues.emplace_back();
		return Residue( _data->residues.back() );
	}

	Frame System::newFrame() const
	{
		if ( _data == nullptr )
		{
			return {};
		}

		_data->frames.emplace_back();
		return Frame( _data->frames.back() );
	}

	bool System::fetch( Atom & p_out, const AtomId & id ) const noexcept
	{
		if ( _data == nullptr )
		{
			return false;
		}

		if ( _data->atoms.contains( id ) )
		{
			p_out = Atom( _data->atoms.at( id ) );
			return true;
		}
		return false;
	}

	bool System::fetch( Frame & p_out, const size_t & p_index ) const noexcept
	{
		if ( _data == nullptr )
		{
			return false;
		}

		if ( _data->frames.size() > p_index )
		{
			auto it_frame = std::next( _data->frames.begin(), p_index );
			p_out		  = Frame( *it_frame );
			return true;
		}
		return false;
	}

	// Pimpl pattern method forwarding vv

	void ChemfilesTrajectory::Del::operator()( ChemfilesTrajectory::Impl * const p_ ) noexcept
	{
		// NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
		delete p_;
	}

	ChemfilesTrajectory::ChemfilesTrajectory() : _( new Impl ) {}

	void ChemfilesTrajectory::close() { _->close(); }

	void ChemfilesTrajectory::setWriteDestination( FilePath p_ ) noexcept { _->setWriteDestination( std::move( p_ ) ); }

	void ChemfilesTrajectory::setWriteFormat( const E_FILE_FORMATS p_ ) noexcept { _->setWriteFormat( p_ ); }

	System ChemfilesTrajectory::system() noexcept { return _->system(); }

} // namespace VTX::IO::Writer
