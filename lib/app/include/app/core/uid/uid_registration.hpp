#ifndef __VTX_APP_CORE_UID_UID_REGISTRATION__
#define __VTX_APP_CORE_UID_UID_REGISTRATION__

#include "app/core/uid/uid.hpp"
#include <mutex>
#include <util/generic/base_static_singleton.hpp>
#include <util/math/range.hpp>
#include <util/math/range_list.hpp>

namespace VTX::App::Core::UID
{
	class UIDRegistration
	{
	  public:
		UIDRegistration() {};
		~UIDRegistration() = default;

		inline const uid	  registerValue() { return _reserveValue(); }
		inline const UIDRange registerRange( const uid p_count ) { return _reserveRange( p_count ); }

		inline void unregister( const uid p_uid ) { _freeValue( p_uid ); }
		inline void unregister( const UIDRange & p_range ) { _freeRange( p_range ); }

		void clear();

	  private:
		Util::Math::RangeList<uid> _availableUIDs = Util::Math::RangeList<uid>( { UIDRange( 0, INVALID_UID - 1 ) } );
		std::mutex				   _idMutex;

		uid		 _reserveValue();
		UIDRange _reserveRange( const uid p_count );

		void _freeValue( const uint p_value );
		void _freeRange( const UIDRange & p_range );
	};
} // namespace VTX::App::Core::UID

#endif
