#ifndef __VTX_SCENE_LOADER__
#define __VTX_SCENE_LOADER__

#include "base_worker.hpp"
#include <map>
#include <util/types.hpp>
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
