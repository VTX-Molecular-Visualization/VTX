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
			explicit Open( Path * p_path ) { _paths.emplace_back( p_path ); }
			explicit Open( const std::vector<Path *> & p_path ) : _paths( p_path ) {}

			virtual void execute() override { VTX_WORKER( new Worker::Loader( _paths ) ); };

		  private:
			std::vector<Path *> _paths = std::vector<Path *>();
		};
	} // namespace Action
} // namespace VTX
#endif
