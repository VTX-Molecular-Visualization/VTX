#ifndef __VTX_ACTION_PATH_EXPORT_VIDEO__
#define __VTX_ACTION_PATH_EXPORT_VIDEO__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/path.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class PathExportVideo : public BaseAction
		{
		  public:
			PathExportVideo( Model::Path * const p_path ) : _path( p_path ) {}

			virtual void execute() override { VTXApp::get().goToState( ID::State::EXPORT, _path ); };

		  private:
			Model::Path * const _path;
		};
	} // namespace Action
} // namespace VTX
#endif
