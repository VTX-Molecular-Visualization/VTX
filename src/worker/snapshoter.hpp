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
			enum class MODE : int
			{
				GL,
				RT
			};

			bool takeSnapshotGL( const IO::Path & p_path ) const;
			bool takeSnapshotRT( const IO::Path & p_path ) const;

		  private:
		};
	} // namespace Worker
} // namespace VTX
#endif
