#ifndef __VTX_UTIL_GENERIC_BASE_NAMEABLE__
#define __VTX_UTIL_GENERIC_BASE_NAMEABLE__

#include "util/types.hpp"

namespace VTX::Util::Generic
{
	// TODO: remove or use concept.
	class BaseNameable
	{
	  public:
		BaseNameable()			= default;
		virtual ~BaseNameable() = default;

		virtual Name getName() const = 0;
	};
} // namespace VTX::Util::Generic

#endif
