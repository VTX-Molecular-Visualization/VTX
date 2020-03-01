#ifndef __VTX_SNAPSHOTER__
#define __VTX_SNAPSHOTER__

#ifdef _MSC_VER
#pragma once
#endif

#include "io/path.hpp"

namespace VTX
{
	namespace Worker
	{
		class Snapshoter
		{
		  public:
			bool takeSnapshot( const IO::Path & p_path ) const;

		  private:
		};
	} // namespace Worker
} // namespace VTX
#endif
