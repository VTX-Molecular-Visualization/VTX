#ifndef __VTX_APP_UID_UID__
#define __VTX_APP_UID_UID__

#include <limits>
#include <util/math/range.hpp>
#include <util/types.hpp>

namespace VTX::App::Uid
{
	using uid	   = uint;
	using UIDRange = Util::Math::Range<uid>;

	inline const uid INVALID_UID = 0;
} // namespace VTX::App::Uid

#endif
