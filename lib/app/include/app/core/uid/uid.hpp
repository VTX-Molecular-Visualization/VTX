#ifndef __VTX_APP_CORE_UID_UID__
#define __VTX_APP_CORE_UID_UID__

#include <limits>
#include <util/math/range.hpp>
#include <util/types.hpp>

namespace VTX::App::Core::UID
{
	using uid	   = uint;
	using UIDRange = Util::Math::Range<uid>;

	inline const uid INVALID_UID = std::numeric_limits<uid>::max();
} // namespace VTX::App::Core::UID

#endif
