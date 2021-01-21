#ifndef __VTX_SAVER__
#define __VTX_SAVER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_worker.hpp"
#include "define.hpp"

namespace VTX
{
	namespace Worker
	{
		class Saver : public Worker::BaseWorker
		{
		  public:
			explicit Saver( FilePath * const p_path ) : _path( p_path ) {}
			~Saver() {}

			virtual void work() override;

		  private:
			FilePath * const _path;
		};
	} // namespace Worker
} // namespace VTX
#endif
