#ifndef __VTX_SCENE_LOADER__
#define __VTX_SCENE_LOADER__

#include "base_worker.hpp"
#include <lib/util/src/filepath.hpp>
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
			explicit SceneLoader( const std::vector<IO::FilePath> & p_paths ) : _paths( p_paths ) {}

		  protected:
			void _run() override;

		  private:
			const std::vector<IO::FilePath> _paths;
		};
	} // namespace Worker
} // namespace VTX
#endif
