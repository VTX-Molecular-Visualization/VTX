#ifndef __VTX_SAVER__
#define __VTX_SAVER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_thread.hpp"
#include "define.hpp"

namespace VTX
{
	namespace Worker
	{
		class Saver : public Worker::BaseThread
		{
			Q_OBJECT

		  public:
			explicit Saver( FilePath * const p_path ) : _path( p_path ) {}
			~Saver() {}

		  protected:
			uint _run() override;

		  private:
			FilePath * const _path;
		};
	} // namespace Worker
} // namespace VTX
#endif
