#ifndef __VTX_SNAPSHOTER__
#define __VTX_SNAPSHOTER__

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

		explicit Snapshoter( const MODE & p_mode, const IO::FilePath & p_path );
		explicit Snapshoter( const MODE &					 p_mode,
							 const IO::FilePath &			 p_path,
							 const IO::Struct::ImageExport & p_exportData );
		explicit Snapshoter( const MODE &								 p_mode,
							 const IO::FilePath &						 p_path,
							 const IO::Struct::ImageExport::RESOLUTION & p_resolution );
		explicit Snapshoter( const MODE &		  p_mode,
							 const IO::FilePath & p_path,
							 const uint			  p_width,
							 const uint			  p_height );

	  protected:
		void _run() override
		{
			switch ( _mode )
			{
			case MODE::GL: _takeSnapshotGL(); break;
			default: break;
			}
		}

	  private:
		const void _takeSnapshotGL() const;

		void _addWatermark( QImage & ) const;

	  private:
		const MODE		   _mode;
		const IO::FilePath _path;

		IO::Struct::ImageExport _exportData;
	};
} // namespace VTX::Worker
#endif
