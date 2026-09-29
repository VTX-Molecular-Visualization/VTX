#ifndef __VTX_UTIL_UID_POOL__
#define __VTX_UTIL_UID_POOL__

#include <mutex>
#include <util/exceptions.hpp>
#include <util/math/range_list.hpp>

namespace VTX
{
	/**
	 * @brief UIDs.
	 */
	using UID32 = uint32_t;
	using UID64 = uint64_t;
} // namespace VTX

namespace VTX::Util::Uid
{
	/**
	 * @brief Pool managing unique identifiers by type.
	 */
	template<typename UID>
	class Pool
	{
	  public:
		using UIDRange	   = Math::Range<UID>;
		using UIDRangeList = Math::RangeList<UID>;

		/**
		 * @brief Constructor.
		 */
		Pool() { clear(); }

		/**
		 * @brief Register a single UID.
		 */
		UID registerValue()
		{
			const std::scoped_lock guard( _mutex );

			if ( _available.isEmpty() )
			{
				throw VTXException( "Unable to reserve UID." );
			}

			const UID res = _available.rangeBegin()->getFirst();
			_available.removeValue( res );
			return res;
		}

		/**
		 * @brief Register a range of UIDs.
		 */
		UIDRange registerRange( UID p_count )
		{
			const std::scoped_lock guard( _mutex );

			auto it = _available.rangeBegin();
			while ( it != _available.rangeEnd() )
			{
				if ( it->getCount() >= p_count )
				{
					auto res = UIDRange::fromFirstCount( it->getFirst(), p_count );
					_available.removeRange( res );
					return res;
				}
				++it;
			}

			throw VTXException( "Unable to reserve UID range." );
		}

		/**
		 * @brief Unregister a single UID.
		 */
		void unregister( const UID p_value )
		{
			const std::scoped_lock guard( _mutex );
			_available.addValue( p_value );
		}

		/**
		 * @brief Unregister a range of UIDs.
		 */
		void unregister( const UIDRange & p_range )
		{
			const std::scoped_lock guard( _mutex );
			_available.addRange( p_range );
		}

		/**
		 * @brief Clear the pool.
		 */
		void clear()
		{
			const std::scoped_lock guard( _mutex );
			_available = UIDRangeList( { Math::Range<UID>( UID( 1 ), std::numeric_limits<UID>::max() ) } );
			_available.removeValue( UID( INVALID_UID ) );
		}

	  private:
		/**
		 * @brief Available UIDs.
		 */
		UIDRangeList _available;

		/**
		 * @brief Mutex for thread safety.
		 */
		std::mutex _mutex;
	};
} // namespace VTX::Util::Uid

#endif
