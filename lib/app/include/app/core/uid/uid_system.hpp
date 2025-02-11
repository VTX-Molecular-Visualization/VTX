#ifndef __VTX_APP_CORE_UID_UID_SYSTEM__
#define __VTX_APP_CORE_UID_UID_SYSTEM__

#include "app/core/uid/uid.hpp"
#include <mutex>
#include <util/math/range.hpp>
#include <util/math/range_list.hpp>
#include <util/singleton.hpp>

namespace VTX::App::Core::UID
{
	class UIDSystem
	{
	  private:
		inline static const Util::Math::RangeList<uid> DEFAULT_RANGE_LIST()
		{
			Util::Math::RangeList<uid> res
				= Util::Math::RangeList<uid>( { UIDRange( 0, std::numeric_limits<uid>::max() ) } );
			res.removeValue( INVALID_UID );

			return res;
		}

	  public:
		UIDSystem() {};
		~UIDSystem() = default;

		inline const uid	  registerValue() { return _reserveValue(); }
		inline const UIDRange registerRange( const uid p_count ) { return _reserveRange( p_count ); }

		inline void unregister( const uid p_uid ) { _freeValue( p_uid ); }
		inline void unregister( const UIDRange & p_range ) { _freeRange( p_range ); }

		void clear();

	  private:
		Util::Math::RangeList<uid> _availableUIDs = DEFAULT_RANGE_LIST();
		std::mutex				   _idMutex;

		uid		 _reserveValue();
		UIDRange _reserveRange( const uid p_count );

		void _freeValue( const uint p_value );
		void _freeRange( const UIDRange & p_range );
	};
} // namespace VTX::App::Core::UID

namespace VTX::App
{
	// Access to UIDSystem to get / release UIDs.
	inline Core::UID::UIDSystem & UID_SYSTEM() { return Util::Singleton<App::Core::UID::UIDSystem>::get(); }
} // namespace VTX::App

#endif
