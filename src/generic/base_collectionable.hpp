#ifndef __VTX_BASE_COLLECTIONABLE__
#define __VTX_BASE_COLLECTIONABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_namable.hpp"

namespace VTX
{
	namespace Generic
	{
		class BaseCollectionable : public BaseNamable
		{
		  public:
			virtual void initItem() {}
		};
	} // namespace Generic
} // namespace VTX
#endif
