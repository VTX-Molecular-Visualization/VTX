#ifndef __VTX_ACTION_OPEN__
#define __VTX_ACTION_OPEN__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/molecule.hpp"
#include "vtx_app.hpp"
#include "worker/loader.hpp"
#include <vector>

namespace VTX
{
	namespace Action
	{
		class Open : public BaseAction
		{
		  public:
			// explicit Open( const std::string & p_path ) { _paths.emplace_back( new IO::Path( p_path ) ); }
			explicit Open( IO::Path * p_path ) { _paths.emplace_back( p_path ); }
			explicit Open( const std::vector<IO::Path *> & p_path ) : _paths( p_path ) {}

			virtual void execute() override { VTX_WORKER( new Worker::Loader( _paths ) ); };

		  private:
			std::vector<IO::Path *> _paths = std::vector<IO::Path *>();
		};
	} // namespace Action
} // namespace VTX
#endif
