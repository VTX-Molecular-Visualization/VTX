#ifndef __VTX_APP_OLD_INTERNAL_WORKER_SCENE_LOADER__
#define __VTX_APP_OLD_INTERNAL_WORKER_SCENE_LOADER__

#include "app/old/application/_fwd.hpp"
#include "app/old/core/worker/base_worker.hpp"
#include <map>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Old::Internal::Worker
{
	class SceneLoader : public App::Old::Core::Worker::BaseWorker
	{
	  public:
		explicit SceneLoader( const std::vector<FilePath> & p_paths ) : _paths( p_paths ) {}

	  protected:
		void _run() override;

	  private:
		const std::vector<FilePath> _paths;
	};
} // namespace VTX::App::Old::Internal::Worker

#endif
