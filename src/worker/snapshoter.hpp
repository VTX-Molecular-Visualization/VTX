#ifndef __VTX_SNAPSHOTER__
#define __VTX_SNAPSHOTER__

#include "base_worker.hpp"
#include "io/filepath.hpp"
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
				GL
			};

			explicit Snapshoter( const MODE & p_mode, const IO::FilePath & p_path );

			explicit Snapshoter( const MODE &		  p_mode,
								 const IO::FilePath & p_path,
								 const uint			  p_width,
								 const uint			  p_height ) :
				_mode( p_mode ),
				_path( p_path ), _width( p_width ), _height( p_height )
			{
			}

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
			const uint		   _quality = 0;
		};
	} // namespace Worker
} // namespace VTX
#endif
