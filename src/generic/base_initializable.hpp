#ifndef __VTX_BASE_INITIALIZABLE__
#define __VTX_BASE_INITIALIZABLE__

#ifdef _MSC_VER
#pragma once
#endif

namespace VTX
{
	namespace Generic
	{
		class BaseInitializable
		{
		  public:
			virtual ~BaseInitializable() = default;

			virtual void init() {}
		};
	} // namespace Generic
} // namespace VTX
#endif
