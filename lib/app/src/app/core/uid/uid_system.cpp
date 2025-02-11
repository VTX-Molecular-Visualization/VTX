#include "app/core/uid/uid_system.hpp"
#include <util/exceptions.hpp>

namespace VTX::App::Core::UID
{
	void UIDSystem::clear()
	{
		std::lock_guard<std::mutex> guard( _idMutex );
		_availableUIDs = DEFAULT_RANGE_LIST();
	}

	uid UIDSystem::_reserveValue()
	{
		std::lock_guard<std::mutex> guard( _idMutex );

		if ( _availableUIDs.isEmpty() )
			throw VTXException( "Unable to reserve UID." );

		const uid res = _availableUIDs.rangeBegin()->getFirst();
		_availableUIDs.removeValue( res );

		return res;
	}
	UIDRange UIDSystem::_reserveRange( const uid p_count )
	{
		std::lock_guard<std::mutex> guard( _idMutex );

		auto it = _availableUIDs.rangeBegin();

		while ( it != _availableUIDs.rangeEnd() )
		{
			if ( it->getCount() >= p_count )
			{
				const UIDRange res = UIDRange( it->getFirst(), p_count );
				_availableUIDs.removeRange( res );

				return res;
			}

			it++;
		}

		throw VTXException( "Unable to reserve UID range." );
	}

	void UIDSystem::_freeValue( const uint p_value )
	{
		std::lock_guard<std::mutex> guard( _idMutex );
		_availableUIDs.addValue( p_value );
	}
	void UIDSystem::_freeRange( const UIDRange & p_range )
	{
		std::lock_guard<std::mutex> guard( _idMutex );
		_availableUIDs.addRange( p_range );
	}
} // namespace VTX::App::Core::UID
