#ifndef __VTX_WORKER_REPRESENTATION_LOADER__
#define __VTX_WORKER_REPRESENTATION_LOADER__

#include "base_worker.hpp"
#include "util/filesystem.hpp"
#include <map>
#include <vector>

namespace VTX
{
	namespace Model::Representation
	{
		class Representation;
		class RepresentationLibrary;
	} // namespace Model::Representation

	namespace Worker
	{
		class RepresentationLibraryLoader : public Worker::BaseWorker
		{
		  public:
			explicit RepresentationLibraryLoader( Model::Representation::RepresentationLibrary & p_library ) :
				RepresentationLibraryLoader( p_library, Util::Filesystem::getRepresentationsLibraryDir() )
			{
			}
			explicit RepresentationLibraryLoader( Model::Representation::RepresentationLibrary & p_library,
												  const IO::FilePath &							 p_path ) :
				_path( p_path ),
				_library( p_library )

			{
			}

			inline void activeNotify( const bool p_notify ) { _notify = p_notify; };
			inline void activeRepresentationRestoration( const bool p_restore ) { _restore = p_restore; };

		  protected:
			void _run() override;

		  private:
			IO::FilePath								   _path;
			Model::Representation::RepresentationLibrary & _library;
			bool										   _notify	= true;
			bool										   _restore = true;
		};

		class RepresentationLoader : public Worker::BaseWorker
		{
		  public:
			explicit RepresentationLoader( const IO::FilePath & p_paths ) { _paths.emplace_back( p_paths ); }
			explicit RepresentationLoader( const std::vector<IO::FilePath> & p_paths ) : _paths( p_paths ) {}

		  protected:
			void _run() override;

		  private:
			std::vector<IO::FilePath> _paths = std::vector<IO::FilePath>();
		};

	} // namespace Worker
} // namespace VTX
#endif
