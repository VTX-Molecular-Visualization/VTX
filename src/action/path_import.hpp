#ifndef __VTX_ACTION_PATH_IMPORT__
#define __VTX_ACTION_PATH_IMPORT__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "util/filesystem.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class PathImport : public BaseAction
		{
		  public:
			explicit PathImport( Model::Path * const p_path ) : _path( p_path ) {}

			virtual void execute() override { _path->load( Util::Filesystem::getPathsPath( "path.txt" ) ); }

		  private:
			Model::Path * const _path;
		};
	} // namespace Action
} // namespace VTX
#endif
