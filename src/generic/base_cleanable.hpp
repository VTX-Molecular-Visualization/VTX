#ifndef __VTX_BASE_CLEANABLE__
#define __VTX_BASE_CLEANABLE__

#ifdef _MSC_VER
#pragma once
#endif

namespace VTX
{
	namespace Generic
	{
		class BaseCleanable
		{
		  public:
			virtual ~BaseCleanable() = default;

			virtual void clean() {}
		};
	} // namespace Generic
} // namespace VTX
#endif
