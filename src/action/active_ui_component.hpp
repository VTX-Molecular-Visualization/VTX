#ifndef __VTX_ACTION_ACTIVE_UI_COMPONENT__
#define __VTX_ACTION_ACTIVE_UI_COMPONENT__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class ActiveUIComponent : public BaseAction
		{
		  public:
			explicit ActiveUIComponent( const std::string & p_name, const bool p_active ) : _name(p_name), _active( p_active ) {}

			virtual void execute() override { VTXApp::get().getUI().getComponentByName( _name )->setVisible( _active ); };

		  private:
			const std::string & _name;
			const bool _active;
		};
	} // namespace Action
} // namespace VTX
#endif
