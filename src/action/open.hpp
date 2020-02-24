#ifndef __VTX_ACTION_OPEN__
#define __VTX_ACTION_OPEN__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/molecule.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class Open : public BaseAction
		{
		  public:
			explicit Open( const IO::Path & p_path ) : _path( p_path ) {}

			virtual void execute() override { VTXApp::get().goToState( ID::State::LOAD, (void *)&_path ); };

		  private:
			const IO::Path & _path;
		};
	} // namespace Action
} // namespace VTX
#endif
