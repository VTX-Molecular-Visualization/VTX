#ifndef __VTX_ACTION_EXPORT_PATH__
#define __VTX_ACTION_EXPORT_PATH__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ActionExportPath : public BaseAction
		{
		  public:
			virtual void execute() override
			{
				VTXApp::get().getScene().getPaths()[ 0 ]->save( PATHS_DIR + "path.txt" );
			}
		};
	} // namespace Action
} // namespace VTX
#endif
