#ifndef __VTX_ACTION_ACTIVE_RENDERER__
#define __VTX_ACTION_ACTIVE_RENDERER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ActiveRenderer : public BaseAction
		{
		  public:
			explicit ActiveRenderer( const bool p_active ) : _active( p_active ) {}

			virtual void execute() override { VTX_SETTING().activeRenderer = _active; }

		  private:
			const bool _active;
		};
	} // namespace Action
} // namespace VTX
#endif
