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
		size_t operator()( const VTX::IO::Writer::AtomId & p_ ) noexcept { return p_.value; }
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
	};
	using AtomCollection			= std::map<AtomId, _Atom>;
	using AtomCoordinatesCollection = std::unordered_map<const _Atom *, AtomCoordinates>;
	using AtomPtrCollection			= std::list<const _Atom *>;

	struct _Residue
	{
		PropertyCollection properties;
		AtomPtrCollection  residues;
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
		_Atom *		 a1;
		_Atom *		 a2;
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
		void figureFormat( const FilePath & dest, Chemfiles::E_FILE_FORMATS & format )
		{
			if ( not dest.has_extension() )
				return;
			std::string extension = dest.extension().string();
			VTX::Util::String::toUpper( extension );

			if ( extension == ".MMCIF" )
				format = Chemfiles::E_FILE_FORMATS::mmcif;
			else if ( extension == ".PDB" )
				format = Chemfiles::E_FILE_FORMATS::pdb;
			else if ( extension == ".MMTF" )
				format = Chemfiles::E_FILE_FORMATS::mmtf;
			else if ( extension == ".MOLDEN" )
				format = Chemfiles::E_FILE_FORMATS::molden;
			else if ( extension == ".CIF" )
				format = Chemfiles::E_FILE_FORMATS::cif;
			else if ( extension == ".GRO" )
				format = Chemfiles::E_FILE_FORMATS::gro;
			else if ( extension == ".MOL2" )
				format = Chemfiles::E_FILE_FORMATS::mol2;
			else if ( extension == ".SDf" )
				format = Chemfiles::E_FILE_FORMATS::sdf;
			else if ( extension == ".SMI" )
				format = Chemfiles::E_FILE_FORMATS::smi;
			else if ( extension == ".XYZ" )
				format = Chemfiles::E_FILE_FORMATS::xyz;
			else if ( extension == ".CML" )
				format = Chemfiles::E_FILE_FORMATS::cml;
			else if ( extension == ".CSSR" )
				format = Chemfiles::E_FILE_FORMATS::cssr;
			else if ( extension == ".NC" )
				format = Chemfiles::E_FILE_FORMATS::nc;
			else if ( extension == ".DCD" )
				format = Chemfiles::E_FILE_FORMATS::nc;
			else if ( extension == ".LAMMPSTRJ" )
				format = Chemfiles::E_FILE_FORMATS::lammpstrj;
			else if ( extension == ".ARC" )
				format = Chemfiles::E_FILE_FORMATS::arc;
			else if ( extension == ".TRR" )
				format = Chemfiles::E_FILE_FORMATS::trr;
			else if ( extension == ".XTC" )
				format = Chemfiles::E_FILE_FORMATS::xtc;
			else if ( extension == ".TNG" )
				format = Chemfiles::E_FILE_FORMATS::tng;
			else if ( extension == ".TRJ" )
				format = Chemfiles::E_FILE_FORMATS::trj;
		}
		const char * string( const Chemfiles::E_FILE_FORMATS & format ) noexcept
		{
			switch ( format )
			{
			case Chemfiles::E_FILE_FORMATS::pdb: return "pdb";
			case Chemfiles::E_FILE_FORMATS::mmcif: return "mmcif";
			case Chemfiles::E_FILE_FORMATS::mmtf: return "mmtf";
			case Chemfiles::E_FILE_FORMATS::molden: return "molden";
			case Chemfiles::E_FILE_FORMATS::cif: return "cif";
			case Chemfiles::E_FILE_FORMATS::gro: return "gro";
			case Chemfiles::E_FILE_FORMATS::mol2: return "mol2";
			case Chemfiles::E_FILE_FORMATS::sdf: return "sdf";
			case Chemfiles::E_FILE_FORMATS::smi: return "smi";
			case Chemfiles::E_FILE_FORMATS::xyz: return "xyz";
			case Chemfiles::E_FILE_FORMATS::cml: return "cml";
			case Chemfiles::E_FILE_FORMATS::cssr: return "cssr";
			case Chemfiles::E_FILE_FORMATS::nc: return "nc";
			case Chemfiles::E_FILE_FORMATS::dcd: return "dcd";
			case Chemfiles::E_FILE_FORMATS::lammpstrj: return "lammpstrj";
			case Chemfiles::E_FILE_FORMATS::arc: return "arc";
			case Chemfiles::E_FILE_FORMATS::trr: return "trr";
			case Chemfiles::E_FILE_FORMATS::xtc: return "xtc";
			case Chemfiles::E_FILE_FORMATS::tng: return "tng";
			case Chemfiles::E_FILE_FORMATS::trj: return "trj";
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
		void writeFile( const FilePath & dest, const Chemfiles::E_FILE_FORMATS & format, _System & system )
		{
			::chemfiles::Trajectory cf_traj( dest.string(), 'w', string( format ) );

			::chemfiles::Frame cf_frame;
			cf_frame.reserve( system.atoms.size() );
			transferProperties( cf_frame, system.properties );
		}

	} // namespace

	class Chemfiles::_Impl
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

	// Residue
	Residue::Residue( _Residue & p_ ) : _data( &p_ ) {}
	void Residue::set( Property p_ ) noexcept
	{
		if ( _data == nullptr )
			return;

		if ( _data->properties.contains( p_.key ) )
			_data->properties.at( p_.key ) = p_.value;
		else
			_data->properties.emplace( p_.key, p_.value );
	}

	void Residue::add( Atom & p_ ) noexcept
	{
		if ( _data == nullptr )
			return;

		_AtomInfo atomInfo;
		p_.get( atomInfo );

		_data->residues.emplace_back( atomInfo.dataPtr );
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
			_data->coordinates.emplace( atomInfo.dataPtr, std::move( p_atomCoordinates ) );
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
			_data->properties.emplace( std::move( p_.key ), std::move( p_.value ) );
	}
	Atom System::newAtom( AtomId p_atomId ) noexcept
	{
		if ( _data == nullptr )
			return {};
		if ( not _data->atoms.contains( p_atomId ) )
		{
			_data->atoms.emplace( p_atomId.value, _Atom() );
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
		if ( not _data->atoms.contains( p_l ) )
			vtxBondThrow( p_l );
		if ( not _data->atoms.contains( p_r ) )
			vtxBondThrow( p_r );

		_data->bonds.emplace_back( _Bond {
			.a1 = &_data->atoms.at( p_l ), .a2 = &_data->atoms.at( p_r ), .bondOrder = std::move( p_bondOrder ) } );
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

	void Chemfiles::Del::operator()( Chemfiles::_Impl * p_ ) noexcept { delete p_; }
	Chemfiles::Chemfiles() : _( new _Impl ) {}

	void Chemfiles::setWriteDestination( FilePath p_ ) noexcept { _->setWriteDestination( std::move( p_ ) ); }

	void Chemfiles::setWriteFormat( E_FILE_FORMATS p_ ) noexcept { _->setWriteFormat( std::move( p_ ) ); }

	System Chemfiles::system() noexcept { return _->system(); }

} // namespace VTX::IO::Writer
