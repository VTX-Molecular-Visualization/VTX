#ifndef __VTX_SCENE_LOADER__
#define __VTX_SCENE_LOADER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_worker.hpp"
#include "define.hpp"
#include <map>
#include <vector>

namespace VTX
{
	namespace Object3D
	{
		class Scene;
	}

	namespace Worker
	{
		class SceneLoader : public Worker::BaseWorker
		{
		  public:
			explicit SceneLoader( const std::vector<FilePath> & p_paths ) : _paths( p_paths ) {}

		  protected:
			void _run() override;

		  private:
			const std::vector<FilePath> _paths;
		};
	} // namespace Worker
} // namespace VTX
#endif
