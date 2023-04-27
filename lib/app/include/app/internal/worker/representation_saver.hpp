#ifndef __VTX_WORKER_REPRESENTATION_SAVER__
#define __VTX_WORKER_REPRESENTATION_SAVER__

#include "app/application/representation/_fwd.hpp"
#include "app/core/worker/base_thread.hpp"
#include "app/core/worker/base_worker.hpp"
#include <util/types.hpp>

namespace VTX::Worker
{
	/*
	class RepresentationSaverThread : public App::Core::Worker::BaseThread
	{
		// Q_OBJECT

	  public:
		explicit RepresentationSaverThread( const App::Application::Representation::RepresentationPreset * const
	p_representation ) : _representation( p_representation )
		{
		}

	  protected:
		uint _run() override;

	  private:
		const App::Application::Representation::RepresentationPreset * const _representation;
	};
	*/

	class RepresentationSaver : public App::Core::Worker::BaseWorker
	{
	  public:
		explicit RepresentationSaver( const App::Application::Representation::RepresentationPreset * const p_representation,
									  const FilePath &												 p_path ) :
			_representation( p_representation ),
			_path( p_path )
		{
		}

	  protected:
		void _run() override;

	  private:
		const App::Application::Representation::RepresentationPreset * const _representation;
		const FilePath												   _path;
	};
} // namespace VTX::Worker
#endif
