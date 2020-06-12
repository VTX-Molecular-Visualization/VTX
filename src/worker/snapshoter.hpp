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

			bool takeSnapshotGL( const Path & p_path ) const;
			bool takeSnapshotRTCPU( const Path & p_path ) const;
			bool takeSnapshotRTOptix( const Path & p_path ) const;

		  private:
		};
	} // namespace Worker
} // namespace VTX
#endif
