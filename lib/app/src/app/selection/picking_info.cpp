#include "app/selection/picking_info.hpp"

namespace VTX::App::Selection
{
	PickingInfo::PickingInfo( const Vec2i p_uids ) : _firstUid( p_uids.x ), _secondUid( p_uids.y ) {}
	PickingInfo::PickingInfo( const uid p_firstUid, const uid p_secondUid ) :
		_firstUid( p_firstUid ), _secondUid( p_secondUid )
	{
	}

	bool operator==( const PickingInfo & p_lhs, const PickingInfo & p_rhs )
	{
		return p_lhs._firstUid == p_rhs._firstUid && p_lhs._secondUid == p_rhs._secondUid;
	}
	bool operator!=( const PickingInfo & p_lhs, const PickingInfo & p_rhs )
	{
		return p_lhs._firstUid != p_rhs._firstUid || p_lhs._secondUid != p_rhs._secondUid;
	}

	bool PickingInfo::hasValue() const { return _firstUid != App::Core::UID::INVALID_UID; }
	bool PickingInfo::hasOneValue() const
	{
		return _firstUid != Core::UID::INVALID_UID && _secondUid == App::Core::UID::INVALID_UID;
	};
	bool PickingInfo::hasTwoValues() const
	{
		return _firstUid != Core::UID::INVALID_UID && _secondUid != App::Core::UID::INVALID_UID;
	}
} // namespace VTX::App::Selection
