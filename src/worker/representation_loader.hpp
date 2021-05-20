#ifndef __VTX_WORKER_REPRESENTATION_LOADER__
#define __VTX_WORKER_REPRESENTATION_LOADER__

#ifdef _MSC_VER
#pragma once
#endif

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
				RepresentationLibraryLoader( p_library,
											 new FilePath( Util::Filesystem::getRepresentationLibraryDirectory() ) )
			{
			}
			explicit RepresentationLibraryLoader( Model::Representation::RepresentationLibrary & p_library,
												  const FilePath * const						 p_path ) :
				_library( p_library ),
				_path( p_path )
			{
			}

			inline void activeNotify( const bool p_notify ) { _notify = p_notify; };

		  protected:
			void _run() override;

		  private:
			const FilePath * const						   _path;
			Model::Representation::RepresentationLibrary & _library;
			bool										   _notify = true;
		};

		class RepresentationLoader : public Worker::BaseWorker
		{
		  public:
			explicit RepresentationLoader( const FilePath & p_paths ) { _paths.emplace_back( &p_paths ); }
			explicit RepresentationLoader( const std::vector<const FilePath *> & p_paths )
			{
				for ( const FilePath * const path : p_paths )
					_paths.emplace_back( path );
			}

		  protected:
			void _run() override;

		  private:
			std::vector<const FilePath *> _paths = std::vector<const FilePath *>();
		};

	} // namespace Worker
} // namespace VTX
#endif