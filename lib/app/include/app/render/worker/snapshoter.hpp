#ifndef __VTX_APP_RENDER_WORKER_SNAPSHOTER__
#define __VTX_APP_RENDER_WORKER_SNAPSHOTER__

// TODO reimplemente that without Qt

#include "app/core/worker/base_worker.hpp"
#include "app/internal/io/serialization/image_export.hpp"
#include "app/render/generic/base_opengl.hpp"
// #include <QImage>

namespace VTX::App::Render::Worker
{
	class Snapshoter : public App::Core::Worker::BaseWorker, public Render::Generic::BaseOpenGL
	{
	  public:
		enum class MODE : int
		{
			GL
		};

		explicit Snapshoter( const MODE &										   p_mode,
							 const FilePath &									   p_path,
							 const App::Internal::IO::Serialization::ImageExport & p_exportData );

		// Temp constructor to bypass call with QImage.
		explicit Snapshoter( const MODE &										   p_mode,
							 void *												   p_imageTarget,
							 const App::Internal::IO::Serialization::ImageExport & p_exportData );

		// explicit Snapshoter( const MODE &					 p_mode,
		//					 QImage *						 p_imageTarget,
		//					 const App::Internal::IO::Serialization::ImageExport & p_exportData );

	  protected:
		void _run() override;

	  private:
		const void _takeSnapshotGL() const;

		// void _addWatermark( QImage & ) const;

	  private:
		const MODE	   _mode;
		const FilePath _path;
		// QImage *	   _imageTarget = nullptr;

		const App::Internal::IO::Serialization::ImageExport _exportData;
	};
} // namespace VTX::App::Render::Worker
#endif
