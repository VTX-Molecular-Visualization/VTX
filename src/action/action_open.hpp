#ifndef __VTX_ACTION_OPEN__
#define __VTX_ACTION_OPEN__

#ifdef _MSC_VER
#pragma once
#endif

#include "../model/model_molecule.hpp"
#include "../vtx_app.hpp"
#include "base_action.hpp"

namespace VTX
{
	namespace Action
	{
		class ActionOpen : public BaseAction
		{
		  public:
			explicit ActionOpen( const std::string & p_path ) : _path( p_path ) {}

			virtual void execute() override { VTXApp::get().goToState( ID::State::LOADING, (void *)&_path ); };

		  private:
			const std::string & _path;
		};
	} // namespace Action
} // namespace VTX
#endif
