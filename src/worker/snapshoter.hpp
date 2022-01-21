#ifndef __VTX_WORKER_SNAPSHOTER__
#define __VTX_WORKER_SNAPSHOTER__

#include "base_worker.hpp"
#include "io/filepath.hpp"
#include "io/struct/image_export.hpp"
#include <QImage>

namespace VTX::Worker
{
	class Snapshoter : public Worker::BaseWorker
	{
	  public:
		enum class MODE : int
		{
			GL
		};

		explicit Snapshoter( const MODE &					 p_mode,
							 const IO::FilePath &			 p_path,
							 const IO::Struct::ImageExport & p_exportData );

		explicit Snapshoter( const MODE &					 p_mode,
							 QImage *						 p_imageTarget,
							 const IO::Struct::ImageExport & p_exportData );

	  protected:
		void _run() override;

	  private:
		const void _takeSnapshotGL() const;

		void _addWatermark( QImage & ) const;

	  private:
		const MODE		   _mode;
		const IO::FilePath _path;
		QImage *		   _imageTarget = nullptr;

		const IO::Struct::ImageExport _exportData;
	};
} // namespace VTX::Worker
#endif
