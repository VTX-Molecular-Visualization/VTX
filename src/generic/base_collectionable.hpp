#ifndef __VTX_BASE_COLLECTIONABLE__
#define __VTX_BASE_COLLECTIONABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include <string>

namespace VTX
{
	namespace Generic
	{
		class BaseCollectionable
		{
		  public:
			virtual ~BaseCollectionable() = default;

			virtual const std::string & getName() const = 0;
			virtual void				init()			= 0;
			virtual void				clean()			= 0;
		};
	} // namespace Generic
} // namespace VTX
#endif
