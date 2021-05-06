#ifndef __VTX_WORKER_REPRESENTATION_SAVER__
#define __VTX_WORKER_REPRESENTATION_SAVER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_thread.hpp"
#include "base_worker.hpp"

namespace VTX
{
	namespace Model::Representation
	{
		class Representation;
	} // namespace Model::Representation

	namespace Worker
	{
		class RepresentationSaverThread : public Worker::BaseThread
		{
			Q_OBJECT

		  public:
			explicit RepresentationSaverThread( const Model::Representation::Representation * const p_representation ) :
				_representation( p_representation )
			{
			}

		  protected:
			uint _run() override;

		  private:
			const Model::Representation::Representation * const _representation;
		};
		class RepresentationSaver : public Worker::BaseWorker
		{
		  public:
			explicit RepresentationSaver( const Model::Representation::Representation * const p_representation ) :
				_representation( p_representation )
			{
			}

		  protected:
			void _run() override;

		  private:
			const Model::Representation::Representation * const _representation;
		};
	}; // namespace Worker
} // namespace VTX
#endif
