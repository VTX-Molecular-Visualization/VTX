#ifndef __VTX_WORKER_SNAPSHOTER__
#define __VTX_WORKER_SNAPSHOTER__

// TODO reimplemente that without Qt

#include "base_worker.hpp"
#include "old_app/generic/base_opengl.hpp"
#include "old_app/io/struct/image_export.hpp"
// #include <QImage>

namespace VTX::Worker
{
	class Snapshoter : public Worker::BaseWorker, public Generic::BaseOpenGL
	{
	  public:
		enum class MODE : int
		{
			GL
		};

		explicit Snapshoter( const MODE &					 p_mode,
							 const FilePath &				 p_path,
							 const IO::Struct::ImageExport & p_exportData );

		// Temp constructor to bypass call with QImage.
		explicit Snapshoter( const MODE & p_mode, void * p_imageTarget, const IO::Struct::ImageExport & p_exportData );

		// explicit Snapshoter( const MODE &					 p_mode,
		//					 QImage *						 p_imageTarget,
		//					 const IO::Struct::ImageExport & p_exportData );

	  protected:
		void _run() override;

	  private:
		const void _takeSnapshotGL() const;

		// void _addWatermark( QImage & ) const;

	  private:
		const MODE	   _mode;
		const FilePath _path;
		// QImage *	   _imageTarget = nullptr;

		const IO::Struct::ImageExport _exportData;
	};
} // namespace VTX::Worker
#endif
