#ifndef __VTX_LOADER__
#define __VTX_LOADER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_worker.hpp"
#include "model/path.hpp"

namespace VTX
{
	namespace Worker
	{
		class Loader : public Worker::BaseWorker
		{
		  public:
			explicit Loader( const IO::Path * const p_path ) : _path( p_path ) {}
			~Loader() { delete _path; }

			virtual void work() override;

		  private:
			const IO::Path * _path;
		};
	} // namespace Worker
} // namespace VTX
#endif
