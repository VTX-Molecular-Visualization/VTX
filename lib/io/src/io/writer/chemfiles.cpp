#include "io/writer/chemfiles.hpp"
#include <list>
#include <map>
#include <unordered_map>
#include <util/exceptions.hpp>

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

	class Chemfiles::_Impl
	{
		FilePath	   _dest;
		E_FILE_FORMATS _format = E_FILE_FORMATS::none;
		_System		   _system;

	  public:
		inline void	  setWriteDestination( FilePath && p_ ) noexcept { _dest = std::move( p_ ); }
		inline void	  setWriteFormat( E_FILE_FORMATS && p_ ) noexcept { _format = std::move( p_ ); }
		inline System system() noexcept { return _system; }
	};

	Atom::Atom( _Atom & p_ ) : _data( &p_ ) {}
	void Atom::get( _AtomInfo & p_out ) const noexcept { p_out = _AtomInfo { .dataPtr = _data }; }

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
