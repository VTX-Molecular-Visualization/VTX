#ifndef __VTX_SNAPSHOTER__
#define __VTX_SNAPSHOTER__

#include "base_worker.hpp"
#include "io/filepath.hpp"
#include <QImage>

namespace VTX
{
	namespace Worker
	{
		enum class SNAPSHOT_RESOLUTION : int
		{
			_800x600,
			_1024x768,
			_1280x800,
			_1600x900,
			_1920x1080,
			_2048x1152,
			_2560x1440,
			_3840x2160,
			_8KUHD,
			_16K,
			Free,
		};
		inline static const std::vector<std::string> SNAPSHOT_RESOLUTION_STR { "800x600",	"1024x768",	 "1280x800",
																			   "1600x900",	"1920x1080", "2048x1152",
																			   "2560x1440", "3840x2160", "8K",
																			   "16K",		"Free" };

		class Snapshoter : public Worker::BaseWorker
		{
		  public:
			enum class MODE : int
			{
				GL
			};

		  private:
			inline static const std::vector<std::pair<int, int>> RESOLUTION_SIZE {
				{ 800, 600 },	{ 1024, 768 },	{ 1280, 800 },	{ 1600, 900 },	{ 1920, 1080 },
				{ 2048, 1152 }, { 2560, 1440 }, { 3840, 2160 }, { 7680, 4320 }, { 15360, 8640 }
			};

		  public:
			explicit Snapshoter( const MODE & p_mode, const IO::FilePath & p_path );
			explicit Snapshoter( const MODE &				 p_mode,
								 const IO::FilePath &		 p_path,
								 const SNAPSHOT_RESOLUTION & p_resolution );
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
			const uint		   _width;
			const uint		   _height;
			const uint _quality = 0;
		};
	} // namespace Worker
} // namespace VTX
#endif
