#ifndef __VTX_ACTION_NEW__
#define __VTX_ACTION_NEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "../vtx_app.hpp"
#include "base_action.hpp"

namespace VTX
{
	namespace Action
	{
		class ActionNew : public BaseAction
		{
		  public:
			virtual void execute() override { Model::ModelCheckpoint };
			virtual void redo() override;
		};
	} // namespace Action
} // namespace VTX
#endif
