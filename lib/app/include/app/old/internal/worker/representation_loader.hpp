#ifndef __VTX_APP_OLD_INTERNAL_WORKER_REPRESENTATION_LOADER__
#define __VTX_APP_OLD_INTERNAL_WORKER_REPRESENTATION_LOADER__

#include "app/old/application/representation/_fwd.hpp"
#include "app/old/core/worker/base_worker.hpp"
#include "app/old/internal/io/filesystem.hpp"
#include <map>
#include <vector>

namespace VTX::App::Old::Internal::Worker
{
	class RepresentationLibraryLoader : public App::Old::Core::Worker::BaseWorker
	{
	  public:
		explicit RepresentationLibraryLoader( App::Old::Application::Representation::RepresentationLibrary & p_library ) :
			RepresentationLibraryLoader( p_library, App::Old::Internal::IO::Filesystem::getRepresentationsLibraryDir() )
		{
		}
		explicit RepresentationLibraryLoader( App::Old::Application::Representation::RepresentationLibrary & p_library,
											  const FilePath &											p_path ) :
			_path( p_path ),
			_library( p_library )

		{
		}

		inline void activeNotify( const bool p_notify ) { _notify = p_notify; };
		inline void activeRepresentationRestoration( const bool p_restore ) { _restore = p_restore; };

	  protected:
		void _run() override;

	  private:
		FilePath												  _path;
		App::Old::Application::Representation::RepresentationLibrary & _library;
		bool													  _notify  = true;
		bool													  _restore = true;
	};

	class RepresentationLoader : public VTX::App::Old::Core::Worker::BaseWorker
	{
	  public:
		explicit RepresentationLoader( const FilePath & p_paths ) { _paths.emplace_back( p_paths ); }
		explicit RepresentationLoader( const std::vector<FilePath> & p_paths ) : _paths( p_paths ) {}

	  protected:
		void _run() override;

	  private:
		std::vector<FilePath> _paths = std::vector<FilePath>();
	};

} // namespace VTX::App::Old::Internal::Worker
#endif
