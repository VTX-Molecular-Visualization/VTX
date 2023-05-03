#ifndef __VTX_APP_INTERNAL_WORKER_SCENE_LOADER__
#define __VTX_APP_INTERNAL_WORKER_SCENE_LOADER__

#include "app/application/_fwd.hpp"
#include "app/core/worker/base_worker.hpp"
#include <map>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Internal::Worker
{
	class SceneLoader : public App::Core::Worker::BaseWorker
	{
	  public:
		explicit SceneLoader( const std::vector<FilePath> & p_paths ) : _paths( p_paths ) {}

	  protected:
		void _run() override;

	  private:
		const std::vector<FilePath> _paths;
	};
} // namespace VTX::App::Internal::Worker

#endif
