#ifndef __VTX_SNAPSHOTER__
#define __VTX_SNAPSHOTER__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"

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
				RT_CPU,
				RT_OPTIX
			};

			bool takeSnapshotGL( const FilePath & p_path );
			bool takeSnapshotRTCPU( const FilePath & p_path ) const;
			bool takeSnapshotRTOptix( const FilePath & p_path ) const;

		  private:
		};
	} // namespace Worker
} // namespace VTX
#endif
