#ifndef __VTX_SNAPSHOTER__
#define __VTX_SNAPSHOTER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_worker.hpp"
#include "define.hpp"
#include <QImage>

namespace VTX
{
	namespace Worker
	{
		class Snapshoter : public Worker::BaseWorker
		{
			Q_OBJECT

		  public:
			enum class MODE : int
			{
				GL,
				RT_CPU,
				RT_OPTIX
			};

			explicit Snapshoter( const MODE & p_mode, const FilePath & p_path, const QImage & p_image ) :
				_mode( p_mode ), _path( p_path ), _image( p_image )
			{
			}

			uint _run() override
			{
				switch ( _mode )
				{
				case MODE::GL: return _takeSnapshotGL();
				case MODE::RT_CPU: return _takeSnapshotRTCPU();
				case MODE::RT_OPTIX: return _takeSnapshotRTOptix();
				default: return 0;
				}
			}

		  private:
			const bool _takeSnapshotGL() const;
			const bool _takeSnapshotRTCPU() const;
			const bool _takeSnapshotRTOptix() const;

		  private:
			const MODE	   _mode;
			const FilePath _path;
			const QImage   _image;
		};
	} // namespace Worker
} // namespace VTX
#endif
