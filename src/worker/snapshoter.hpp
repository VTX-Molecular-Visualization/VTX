#ifndef __VTX_SNAPSHOTER__
#define __VTX_SNAPSHOTER__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include <QOpenGLFunctions_4_5_Compatibility>

namespace VTX
{
	namespace Worker
	{
		class Snapshoter : public QOpenGLFunctions_4_5_Compatibility
		{
		  public:
			enum class MODE : int
			{
				GL,
				RT_CPU,
				RT_OPTIX
			};

			bool takeSnapshotGL( const Path & p_path );
			bool takeSnapshotRTCPU( const Path & p_path ) const;
			bool takeSnapshotRTOptix( const Path & p_path ) const;

		  private:
		};
	} // namespace Worker
} // namespace VTX
#endif
