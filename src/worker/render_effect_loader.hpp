#ifndef __VTX_WORKER_RENDER_EFFECT_PRESET_LOADER__
#define __VTX_WORKER_RENDER_EFFECT_PRESET_LOADER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_worker.hpp"
#include "define.hpp"
#include "util/filesystem.hpp"
#include <vector>

namespace VTX
{
	namespace Model::Renderer
	{
		class RenderEffectPresetLibrary;
	}

	namespace Worker
	{
		class RenderEffectPresetLibraryLoader : public Worker::BaseWorker
		{
		  public:
			explicit RenderEffectPresetLibraryLoader( Model::Renderer::RenderEffectPresetLibrary & p_library ) :
				RenderEffectPresetLibraryLoader(
					p_library,
					new FilePath( Util::Filesystem::getRenderEffectPresetLibraryDirectory() ) )
			{
			}
			explicit RenderEffectPresetLibraryLoader( Model::Renderer::RenderEffectPresetLibrary & p_library,
													  const FilePath * const					   p_path ) :
				_library( p_library ),
				_path( p_path )
			{
			}

			inline void activeNotify( const bool p_notify ) { _notify = p_notify; };

		  protected:
			void _run() override;

		  private:
			const FilePath * const						 _path;
			Model::Renderer::RenderEffectPresetLibrary & _library;
			bool										 _notify = true;
		};

		class RenderEffectPresetLoader : public Worker::BaseWorker
		{
		  public:
			explicit RenderEffectPresetLoader( const FilePath & p_paths ) { _paths.emplace_back( &p_paths ); }
			explicit RenderEffectPresetLoader( const std::vector<const FilePath *> & p_paths )
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