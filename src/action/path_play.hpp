#ifndef __VTX_ACTION_EXPORT_VIDEO__
#define __VTX_ACTION_EXPORT_VIDEO__

#ifdef _MSC_VER
#pragma once
#endif

#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class PathPlay : public BaseAction
		{
		  public:
			explicit PathPlay( Model::Path * const p_path ) : _path( p_path ) {}
			virtual void execute() override { VTXApp::get().goToState( ID::State::PLAY, _path ); };

		  private:
			Model::Path * const _path;
		};
	} // namespace Action
} // namespace VTX
#endif
