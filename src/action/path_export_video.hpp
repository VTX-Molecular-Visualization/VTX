#ifndef __VTX_ACTION_PATH_EXPORT_VIDEO__
#define __VTX_ACTION_PATH_EXPORT_VIDEO__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/path.hpp"
#include "state/export.hpp"
#include "vtx_app.hpp"
#include "worker/snapshoter.hpp"

namespace VTX
{
	namespace Action
	{
		class PathExportVideo : public BaseAction
		{
		  public:
			explicit PathExportVideo( Model::Path * const p_path, Worker::Snapshoter::MODE p_mode ) :
				_path( p_path ), _mode( p_mode )
			{
			}

			virtual void execute() override
			{
				State::Export::Arg arg;
				arg.mode = _mode;
				arg.path = _path;

				VTXApp::get().goToState( ID::State::EXPORT, &arg );
			};

		  private:
			const Worker::Snapshoter::MODE _mode;
			Model::Path * const			   _path;
		};
	} // namespace Action
} // namespace VTX
#endif
