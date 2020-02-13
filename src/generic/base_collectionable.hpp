#ifndef __VTX_BASE_COLLECTIONABLE__
#define __VTX_BASE_COLLECTIONABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_cleanable.hpp"
#include "base_initializable.hpp"
#include "define.hpp"
#include <string>

namespace VTX
{
	namespace Generic
	{
		class BaseCollectionable : public BaseInitializable, public BaseCleanable
		{
		  public:
			virtual ~BaseCollectionable() = default;

			virtual std::string getName() const = 0;
		};
	} // namespace Generic
} // namespace VTX
#endif
