#ifndef __VTX_SAVER__
#define __VTX_SAVER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_worker.hpp"
#include "io/writer/base_writer.hpp"

namespace VTX
{
	namespace Worker
	{
		class Saver : public Worker::BaseWorker
		{
		  public:
			explicit Saver( Path * const p_path ) : _path( p_path ) {}
			~Saver() {}

			virtual void work() override;

		  private:
			Path * const _path;
		};
	} // namespace Worker
} // namespace VTX
#endif
