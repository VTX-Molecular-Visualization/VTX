#ifndef __VTX_BASE_SERIALIZABLE__
#define __VTX_BASE_SERIALIZABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "io/path.hpp"

namespace VTX
{
	namespace Generic
	{
		class BaseSerializable
		{
		  public:
			virtual ~BaseSerializable() = default;

			virtual void load( const IO::Path & )		= 0;
			virtual void save( const IO::Path & ) const = 0;
		};
	} // namespace Generic
} // namespace VTX
#endif
