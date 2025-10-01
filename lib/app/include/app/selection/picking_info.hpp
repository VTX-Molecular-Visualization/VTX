#ifndef __VTX_APP_SELECTION_PICKING_INFO__
#define __VTX_APP_SELECTION_PICKING_INFO__

#include "app/uid/uid.hpp"
#include <util/types.hpp>

namespace VTX::App::Selection
{
	struct PickingInfo
	{
		using uid = App::Uid::uid;

		PickingInfo() = default;
		PickingInfo( const uid p_firstUid, const uid p_secondUid );
		PickingInfo( const Vec2i p_uids );
		PickingInfo( const PickingInfo & p_source ) = default;

		friend bool operator==( const PickingInfo & p_lhs, const PickingInfo & p_rhs );
		friend bool operator!=( const PickingInfo & p_lhs, const PickingInfo & p_rhs );

		bool hasValue() const;
		bool hasOneValue() const;
		bool hasTwoValues() const;

		uid getFirst() const { return _firstUid; }
		uid getSecond() const { return _secondUid; }

	  private:
		uid _firstUid  = Uid::INVALID_UID;
		uid _secondUid = Uid::INVALID_UID;
	};

} // namespace VTX::App::Selection
#endif
