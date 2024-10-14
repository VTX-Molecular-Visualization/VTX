#include "io/writer/chemfiles.hpp"
#include <algorithm>
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
	struct _AtomInfo
	{
		_Atom * dataPtr;
	};

	struct _Atom
	{
		AtomId		externalId;
		std::string name;
		std::string symbol;
	};
	using AtomCollection			= std::map<AtomId, _Atom>;
	using AtomCoordinatesCollection = std::unordered_map<const _Atom *, AtomCoordinates>;
	using AtomPtrCollection			= std::list<const _Atom *>;

	struct _Residue
	{
		int				   resid;
		std::string		   symbol;
		PropertyCollection properties;
		AtomPtrCollection  atoms;
	};
	using ResidueCollection = std::list<_Residue>;

	struct _Chain
	{
		std::string name;
		std::string id;
	};

	using ChainCollection = std::list<_Chain>; // Used for constant time insertion and memory efficiency
	struct _Frame
	{
		AtomCoordinatesCollection coordinates;
	};
	using FrameCollection = std::list<_Frame>; // Used for constant time insertion and memory efficiency

	struct _Bond
	{
		AtomId		 a1;
		AtomId		 a2;
		E_BOND_ORDER bondOrder;
	};
	using BondCollection = std::list<_Bond>; // Used for constant time insertion and memory efficiency

	struct _System
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
		void figureFormat( const FilePath & dest, ChemfilesTrajectory::E_FILE_FORMATS & format )
		{
			if ( not dest.has_extension() )
				return;
			std::string extension = dest.extension().string();
			VTX::Util::String::toUpper( extension );

			if ( extension == ".MMCIF" )
				format = ChemfilesTrajectory::E_FILE_FORMATS::mmcif;
			else if ( extension == ".PDB" )
				format = ChemfilesTrajectory::E_FILE_FORMATS::pdb;
			else if ( extension == ".MMTF" )
				format = ChemfilesTrajectory::E_FILE_FORMATS::mmtf;
			else if ( extension == ".MOLDEN" )
				format = ChemfilesTrajectory::E_FILE_FORMATS::molden;
			else if ( extension == ".CIF" )
				format = ChemfilesTrajectory::E_FILE_FORMATS::cif;
			else if ( extension == ".GRO" )
				format = ChemfilesTrajectory::E_FILE_FORMATS::gro;
			else if ( extension == ".MOL2" )
				format = ChemfilesTrajectory::E_FILE_FORMATS::mol2;
			else if ( extension == ".SDf" )
				format = ChemfilesTrajectory::E_FILE_FORMATS::sdf;
			else if ( extension == ".SMI" )
				format = ChemfilesTrajectory::E_FILE_FORMATS::smi;
			else if ( extension == ".XYZ" )
				format = ChemfilesTrajectory::E_FILE_FORMATS::xyz;
			else if ( extension == ".CML" )
				format = ChemfilesTrajectory::E_FILE_FORMATS::cml;
			else if ( extension == ".CSSR" )
				format = ChemfilesTrajectory::E_FILE_FORMATS::cssr;
			else if ( extension == ".NC" )
				format = ChemfilesTrajectory::E_FILE_FORMATS::nc;
			else if ( extension == ".DCD" )
				format = ChemfilesTrajectory::E_FILE_FORMATS::nc;
			else if ( extension == ".LAMMPSTRJ" )
				format = ChemfilesTrajectory::E_FILE_FORMATS::lammpstrj;
			else if ( extension == ".ARC" )
				format = ChemfilesTrajectory::E_FILE_FORMATS::arc;
			else if ( extension == ".TRR" )
				format = ChemfilesTrajectory::E_FILE_FORMATS::trr;
			else if ( extension == ".XTC" )
				format = ChemfilesTrajectory::E_FILE_FORMATS::xtc;
			else if ( extension == ".TNG" )
				format = ChemfilesTrajectory::E_FILE_FORMATS::tng;
			else if ( extension == ".TRJ" )
				format = ChemfilesTrajectory::E_FILE_FORMATS::trj;
		}
		const char * string( const ChemfilesTrajectory::E_FILE_FORMATS & format ) noexcept
		{
			switch ( format )
			{
			case ChemfilesTrajectory::E_FILE_FORMATS::pdb: return "PDB";
			case ChemfilesTrajectory::E_FILE_FORMATS::mmcif: return "mmCIF";
			case ChemfilesTrajectory::E_FILE_FORMATS::mmtf: return "MMTF";
			case ChemfilesTrajectory::E_FILE_FORMATS::molden: return "Molden";
			case ChemfilesTrajectory::E_FILE_FORMATS::cif: return "CIF";
			case ChemfilesTrajectory::E_FILE_FORMATS::gro: return "GRO";
			case ChemfilesTrajectory::E_FILE_FORMATS::mol2: return "MOL2";
			case ChemfilesTrajectory::E_FILE_FORMATS::sdf: return "SDF";
			case ChemfilesTrajectory::E_FILE_FORMATS::smi: return "SMI";
			case ChemfilesTrajectory::E_FILE_FORMATS::xyz: return "XYZ";
			case ChemfilesTrajectory::E_FILE_FORMATS::cml: return "CML";
			case ChemfilesTrajectory::E_FILE_FORMATS::cssr: return "CSSR";
			case ChemfilesTrajectory::E_FILE_FORMATS::nc: return "Amber NetCDF";
			case ChemfilesTrajectory::E_FILE_FORMATS::dcd: return "DCD";
			case ChemfilesTrajectory::E_FILE_FORMATS::lammpstrj: return "LAMMPS";
			case ChemfilesTrajectory::E_FILE_FORMATS::arc: return "Tinker";
			case ChemfilesTrajectory::E_FILE_FORMATS::trr: return "TRR";
			case ChemfilesTrajectory::E_FILE_FORMATS::xtc: return "XTC";
			case ChemfilesTrajectory::E_FILE_FORMATS::tng: return "TNG";
			case ChemfilesTrajectory::E_FILE_FORMATS::trj: return "TRJ";
			default: break;
			}
			return "";
		}

		template<typename T>
		inline void setProperty( T & cf, const std::string & key, const std::string & value )
		{
			cf.set( key, value );
		}
		template<typename T>
		inline void setProperty( T & cf, const std::string & key, const double & value )
		{
			cf.set( key, value );
		}
		template<typename T>
		inline void setProperty( T & cf, const std::string & key, const bool & value )
		{
			cf.set( key, value );
		}
		template<typename T>
		void transferProperties( T & cf, const PropertyCollection & props ) noexcept
		{
			for ( auto & prop : props )
				std::visit( [ & ]( auto v ) { setProperty<T>( cf, prop.first, v ); }, prop.second );
		}
		inline void convert( const AtomCoordinates & in, ::chemfiles::Vector3D & out ) noexcept
		{
			out = ::chemfiles::Vector3D( in.x, in.y, in.z );
		}
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
		void writeFile( const FilePath & dest, const ChemfilesTrajectory::E_FILE_FORMATS & format, _System & system )
		{
			::chemfiles::Trajectory cf_traj( dest.string(), 'w', string( format ) );

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
						AtomCoordinates		  coords = it_frame.coordinates.at( it_atom );
						::chemfiles::Vector3D cf_coords;
						convert( coords, cf_coords );
						cf_frame.add_atom( ::chemfiles::Atom( it_atom->name, it_atom->symbol ), cf_coords );
						if ( firstFrameWritten )
						{
							if ( not atomIdCorrespondaceTable.contains( it_atom->externalId ) )
								atomIdCorrespondaceTable.insert( { it_atom->externalId, atomResidueCounter } );
						}
						cf_residue.add_atom( atomResidueCounter++ );
					}
					cf_frame.add_residue( cf_residue );
				}

				for ( auto & it_bond : system.bonds )
				{
					::chemfiles::Bond::BondOrder cf_bondOrder = ::chemfiles::Bond::BondOrder::UNKNOWN;
					convert( it_bond.bondOrder, cf_bondOrder );
					cf_frame.add_bond(
						atomIdCorrespondaceTable.at( it_bond.a1 ),
						atomIdCorrespondaceTable.at( it_bond.a2 ),
						cf_bondOrder
					);
				}

				cf_traj.write( cf_frame );
				firstFrameWritten = false;
			}
			cf_traj.close();
		}

	} // namespace

	class ChemfilesTrajectory::_Impl
	{
		FilePath	   _dest;
		E_FILE_FORMATS _format = E_FILE_FORMATS::none;
		_System		   _system;

	  public:
		inline void	  setWriteDestination( FilePath && p_ ) noexcept { _dest = std::move( p_ ); }
		inline void	  setWriteFormat( E_FILE_FORMATS && p_ ) noexcept { _format = std::move( p_ ); }
		inline System system() noexcept { return _system; }
		_Impl()							   = default;
		_Impl( const _Impl & )			   = delete;
		_Impl( _Impl && )				   = delete;
		_Impl & operator=( const _Impl & ) = delete;
		_Impl & operator=( _Impl && )	   = delete;
		~_Impl()
		{
			if ( _system.frames.empty() )
				return;
			if ( _dest.empty() )
				return;
			if ( _format == E_FILE_FORMATS::none )
				figureFormat( _dest, _format );
			if ( _format == E_FILE_FORMATS::none )
				return;

			writeFile( _dest, _format, _system );
		}
	};

	// Atom
	Atom::Atom( _Atom & p_ ) : _data( &p_ ) {}
	void Atom::get( _AtomInfo & p_out ) const noexcept { p_out = _AtomInfo { .dataPtr = _data }; }

	void Atom::setName( std::string name ) noexcept
	{
		if ( _data == nullptr )
			return;
		_data->name = std::move( name );
	}

	void Atom::setSymbol( std::string symbol ) noexcept
	{
		if ( _data == nullptr )
			return;
		_data->symbol = std::move( symbol );
	}

	// Residue
	Residue::Residue( _Residue & p_ ) : _data( &p_ ) {}
	void Residue::set( Property p_ ) noexcept
	{
		if ( _data == nullptr )
			return;

		if ( _data->properties.contains( p_.key ) )
			_data->properties.at( p_.key ) = p_.value;
		else
			_data->properties.emplace( std::make_pair( p_.key, p_.value ) );
	}

	void Residue::setResId( int p_ ) noexcept
	{
		if ( _data == nullptr )
			return;

		_data->resid = std::move( p_ );
	}

	void Residue::setSymbol( std::string p_symbol ) noexcept
	{
		if ( _data == nullptr )
			return;

		_data->symbol = std::move( p_symbol );
	}

	void Residue::add( Atom & p_ ) noexcept
	{
		if ( _data == nullptr )
			return;

		_AtomInfo atomInfo;
		p_.get( atomInfo );

		_data->atoms.emplace_back( atomInfo.dataPtr );
	}

	// Chain
	Chain::Chain( _Chain & p_ ) : _data( &p_ ) {}
	void Chain::setName( std::string p_ ) noexcept
	{
		if ( _data == nullptr )
			return;
		_data->name = std::move( p_ );
	}
	void Chain::setId( std::string p_ ) noexcept
	{
		if ( _data == nullptr )
			return;
		_data->id = std::move( p_ );
	}
	void Chain::add( Residue & p_res ) noexcept
	{
		if ( _data == nullptr )
			return;

		p_res.set( { "chainname", _data->name } );
		p_res.set( { "chainid", _data->id } );
	}

	// Frame
	Frame::Frame( _Frame & p_ ) : _data( &p_ ) {}
	void Frame::set( const Atom & p_atom, AtomCoordinates p_atomCoordinates ) noexcept
	{
		if ( _data == nullptr )
			return;

		_AtomInfo atomInfo;
		p_atom.get( atomInfo );
		if ( atomInfo.dataPtr == nullptr )
			return;

		if ( not _data->coordinates.contains( atomInfo.dataPtr ) )
			_data->coordinates.emplace( std::make_pair( atomInfo.dataPtr, std::move( p_atomCoordinates ) ) );
		else
			_data->coordinates.at( atomInfo.dataPtr ) = std::move( p_atomCoordinates );
	}

	// System
	System::System( _System & p_ ) : _data( &p_ ) {}
	void System::set( Property p_ ) noexcept
	{
		if ( _data == nullptr )
			return;
		if ( _data->properties.contains( p_.key ) )
			_data->properties.at( p_.key ) = std::move( p_.value );
		else
			_data->properties.emplace( std::make_pair( std::move( p_.key ), std::move( p_.value ) ) );
	}
	Atom System::newAtom( AtomId p_atomId ) noexcept
	{
		if ( _data == nullptr )
			return {};
		if ( not _data->atoms.contains( p_atomId ) )
		{
			_data->atoms.emplace( std::make_pair( p_atomId, _Atom { .externalId = p_atomId } ) );
		}

		return Atom( _data->atoms.at( p_atomId ) );
	}
	namespace
	{
		void vtxBondThrow( const AtomId & atom )
		{
			throw VTXException( "Couldn't create bond because atom <{}> wasn't declared", atom.value );
		}
	} // namespace
	void System::bind( const AtomId & p_l, const AtomId & p_r, E_BOND_ORDER p_bondOrder )
	{
		if ( _data == nullptr )
			return;

		// First we check if the provided AtomId point toward an actual atom.
		if ( not _data->atoms.contains( p_l ) )
			vtxBondThrow( p_l );
		if ( not _data->atoms.contains( p_r ) )
			vtxBondThrow( p_r );

		_data->bonds.emplace_back( _Bond { .a1 = p_l, .a2 = p_r, .bondOrder = std::move( p_bondOrder ) } );
	}
	Chain System::newChain() noexcept
	{
		if ( _data == nullptr )
			return {};
		_data->chains.emplace_back();
		return Chain( _data->chains.back() );
	}
	Residue System::newResidue() noexcept
	{
		if ( _data == nullptr )
			return {};
		_data->residues.emplace_back();
		return Residue( _data->residues.back() );
	}
	Frame System::newFrame() noexcept
	{
		if ( _data == nullptr )
			return {};

		_data->frames.emplace_back();
		return Frame( _data->frames.back() );
	}

	bool System::fetch( Atom & p_out, const AtomId & id ) noexcept
	{
		if ( _data == nullptr )
			return false;

		if ( _data->atoms.contains( id ) )
		{
			p_out = Atom( _data->atoms.at( id ) );
			return true;
		}
		return false;
	}

	// Pimpl pattern method forwarding vv

	void ChemfilesTrajectory::Del::operator()( ChemfilesTrajectory::_Impl * p_ ) noexcept { delete p_; }
	ChemfilesTrajectory::ChemfilesTrajectory() : _( new _Impl ) {}

	void ChemfilesTrajectory::setWriteDestination( FilePath p_ ) noexcept { _->setWriteDestination( std::move( p_ ) ); }

	void ChemfilesTrajectory::setWriteFormat( E_FILE_FORMATS p_ ) noexcept { _->setWriteFormat( std::move( p_ ) ); }

	System ChemfilesTrajectory::system() noexcept { return _->system(); }

} // namespace VTX::IO::Writer
