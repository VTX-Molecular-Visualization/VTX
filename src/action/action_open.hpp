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
			ActionOpen( const std::string & p_path ) : _path( p_path ) {}

			virtual bool canBeUndone() override { return false; }
			virtual void execute() override { VTXApp::get().goToState( State::STATE_NAME::LOADING, (void *)&_path ); };

		  private:
			const std::string & _path;
		};
	} // namespace Action
} // namespace VTX
#endif
