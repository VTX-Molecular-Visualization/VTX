#ifndef __VTX_APP_CORE_UID_POOL__
#define __VTX_APP_CORE_UID_POOL__

#include <mutex>
#include <util/exceptions.hpp>
#include <util/math/range_list.hpp>

namespace VTX::App::Uid
{
	/**
	 * @brief Pool managing unique identifiers by type.
	 */
	template<typename UID>
	class Pool
	{
	  public:
		using UIDRange	   = Util::Math::Range<UID>;
		using UIDRangeList = Util::Math::RangeList<UID>;

		/**
		 * @brief Constructor.
		 */
		Pool() { clear(); }

		/**
		 * @brief Register a single UID.
		 */
		UID registerValue()
		{
			std::lock_guard<std::mutex> guard( _mutex );

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
			std::lock_guard<std::mutex> guard( _mutex );

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
			std::lock_guard<std::mutex> guard( _mutex );
			_available.addValue( p_value );
		}

		/**
		 * @brief Unregister a range of UIDs.
		 */
		void unregister( const UIDRange & p_range )
		{
			std::lock_guard<std::mutex> guard( _mutex );
			_available.addRange( p_range );
		}

		/**
		 * @brief Clear the pool.
		 */
		void clear()
		{
			std::lock_guard<std::mutex> guard( _mutex );
			_available = UIDRangeList( { Util::Math::Range<UID>( UID( 1 ), std::numeric_limits<UID>::max() ) } );
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
} // namespace VTX::App::Uid

#endif
