#ifndef __VTX_BASE_COLLECTIONABLE__
#define __VTX_BASE_COLLECTIONABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include <string>

namespace VTX
{
	namespace Generic
	{
		class BaseCollectionable
		{
		  public:
			virtual ~BaseCollectionable() = default;

			virtual void initItem() {}
			virtual void cleanItem() {}

			virtual std::string getName() const = 0;
		};
	} // namespace Generic
} // namespace VTX
#endif
