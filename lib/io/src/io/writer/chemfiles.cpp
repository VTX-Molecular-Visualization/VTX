#include "io/writer/chemfiles.hpp"
#include <list>
#include <map>
#include <unordered_map>
#include <util/exceptions.hpp>

namespace VTX::IO::Writer
{
	struct _AtomInfo
	{
	};

	struct _Atom
	{
	};
	using AtomCollection = std::map<uint64_t, _Atom>;

	struct _Chain
	{
	};

	using ChainCollection = std::list<_Chain>; // Used for constant time insertion and memory efficiency
	struct _Frame
	{
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
		std::unordered_map<std::string, PropertyValue> properties;
		AtomCollection								   atoms;
		BondCollection								   bonds;
		ChainCollection								   chains;
	};

	class Chemfiles::_Impl
	{
		FilePath	   _dest;
		E_FILE_FORMATS _format = E_FILE_FORMATS::none;

	  public:
		inline void	  setWriteDestination( FilePath && p_ ) noexcept { _dest = std::move( p_ ); }
		inline void	  setWriteFormat( E_FILE_FORMATS && p_ ) noexcept { _format = std::move( p_ ); }
		inline System system() noexcept {}
	};

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
		if ( not _data->atoms.contains( p_atomId.id ) )
		{
			_data->atoms.emplace( p_atomId.id, _Atom() );
		}

		return Atom( _data->atoms.at( p_atomId.id ) );
	}
	namespace
	{
		void vtxBondThrow( const AtomId & atom )
		{
			throw VTXException( "Couldn't create bond because atom <{}> wasn't declared", atom.id );
		}
	} // namespace
	void System::bind( const AtomId & p_l, const AtomId & p_r, E_BOND_ORDER p_bondOrder )
	{
		if ( _data == nullptr )
			return;
		if ( not _data->atoms.contains( p_l.id ) )
			vtxBondThrow( p_l );
		if ( not _data->atoms.contains( p_r.id ) )
			vtxBondThrow( p_r );

		_data->bonds.emplace_back( _Bond { .a1		  = &_data->atoms.at( p_l.id ),
										   .a2		  = &_data->atoms.at( p_r.id ),
										   .bondOrder = std::move( p_bondOrder ) } );
	}
	Chain::Chain( _Chain & p_ ) : _data( &p_ ) {}
	Chain System::newChain() noexcept
	{
		if ( _data == nullptr )
			return;
		_data->chains.emplace_back();
		return Chain( _data->chains.back() );
	}

	// Pimpl pattern method forwarding vv

	void Chemfiles::Del::operator()( Chemfiles::_Impl * p_ ) noexcept { delete p_; }
	Chemfiles::Chemfiles() : _( new _Impl ) {}

	void Chemfiles::setWriteDestination( FilePath p_ ) noexcept { _->setWriteDestination( std::move( p_ ) ); }

	void Chemfiles::setWriteFormat( E_FILE_FORMATS p_ ) noexcept { _->setWriteFormat( std::move( p_ ) ); }

	System Chemfiles::system() noexcept { return _->system(); }

} // namespace VTX::IO::Writer
