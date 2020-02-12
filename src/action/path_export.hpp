#ifndef __VTX_ACTION_PATH_EXPORT__
#define __VTX_ACTION_PATH_EXPORT__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class PathExport : public BaseAction
		{
		  public:
			PathExport( Model::Path * const p_path ) : _path( p_path ) {}

			virtual void execute() override { _path->save( PATHS_DIR + "path.txt" ); }

		  private:
			Model::Path * const _path;
		};
	} // namespace Action
} // namespace VTX
#endif
