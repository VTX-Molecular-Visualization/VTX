#ifndef __VTX_APP_OLD_INTERNAL_WORKER_REPRESENTATION_SAVER__
#define __VTX_APP_OLD_INTERNAL_WORKER_REPRESENTATION_SAVER__

#include "app/old/application/representation/_fwd.hpp"
#include "app/old/core/worker/base_thread.hpp"
#include "app/old/core/worker/base_worker.hpp"
#include <util/types.hpp>

namespace VTX::App::Old::Internal::Worker
{
	/*
	class RepresentationSaverThread : public App::Old::Core::Worker::BaseThread
	{
		// Q_OBJECT

	  public:
		explicit RepresentationSaverThread( const App::Old::Application::Representation::RepresentationPreset * const
	p_representation ) : _representation( p_representation )
		{
		}

	  protected:
		uint _run() override;

	  private:
		const App::Old::Application::Representation::RepresentationPreset * const _representation;
	};
	*/

	class RepresentationSaver : public App::Old::Core::Worker::BaseWorker
	{
	  public:
		explicit RepresentationSaver(
			const App::Old::Application::Representation::RepresentationPreset * const p_representation,
			const FilePath &													 p_path ) :
			_representation( p_representation ),
			_path( p_path )
		{
		}

	  protected:
		void _run() override;

	  private:
		const App::Old::Application::Representation::RepresentationPreset * const _representation;
		const FilePath														 _path;
	};
} // namespace VTX::App::Old::Internal::Worker
#endif
