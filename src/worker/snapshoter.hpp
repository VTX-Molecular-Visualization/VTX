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
		  public:
			enum class MODE : int
			{
				GL,
				RT_CPU,
				RT_OPTIX
			};

			explicit Snapshoter( const MODE & p_mode, const FilePath & p_path ) : _mode( p_mode ), _path( p_path ) {}

		  protected:
			void _run() override
			{
				switch ( _mode )
				{
				case MODE::GL: _takeSnapshotGL(); break;
				case MODE::RT_CPU: _takeSnapshotRTCPU(); break;
				case MODE::RT_OPTIX: _takeSnapshotRTOptix(); break;
				default: break;
				}
			}

		  private:
			const void _takeSnapshotGL() const;
			const void _takeSnapshotRTCPU() const;
			const void _takeSnapshotRTOptix() const;

			void _addWatermark( QImage & ) const;

		  private:
			const MODE	   _mode;
			const FilePath _path;
		};
	} // namespace Worker
} // namespace VTX
#endif
