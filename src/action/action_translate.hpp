#ifndef __VTX_ACTION_TRANSLATE__
#define __VTX_ACTION_TRANSLATE__

#ifdef _MSC_VER
#pragma once
#endif

#include "../model/model_molecule.hpp"
#include "../vtx_app.hpp"
#include "base_action_undonable.hpp"

namespace VTX
{
	namespace Action
	{
		class ActionTranslate : public BaseActionUndonable
		{
		  public:
			explicit ActionTranslate() {}

			virtual void execute() override {};

			virtual void undo() override {}

		  private:
		};
	} // namespace Action
} // namespace VTX
#endif
