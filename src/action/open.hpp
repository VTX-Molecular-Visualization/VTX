#ifndef __VTX_ACTION_OPEN__
#define __VTX_ACTION_OPEN__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/molecule.hpp"
#include "vtx_app.hpp"
#include "worker/loader.hpp"

namespace VTX
{
	namespace Action
	{
		class Open : public BaseAction
		{
		  public:
			explicit Open( const std::string & p_path ) : _path( new IO::Path( p_path ) ) {}
			explicit Open( const IO::Path * const p_path ) : _path( p_path ) {}

			virtual void execute() override { VTX_WORKER( new Worker::Loader( _path ) ); };

		  private:
			const IO::Path * _path;
		};
	} // namespace Action
} // namespace VTX
#endif
