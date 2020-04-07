#ifndef __VTX_ACTION_VIEWPOINT_ADD_ACTION__
#define __VTX_ACTION_VIEWPOINT_ADD_ACTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/viewpoint.hpp"

namespace VTX
{
	namespace Action
	{
		class ViewpointAddAction : public BaseAction
		{
		  public:
			explicit ViewpointAddAction( Model::Viewpoint & p_viewpoint, const std::string & p_action ) :
				_viewpoint( p_viewpoint ), _action( p_action )
			{
			}

			virtual void execute() override { _viewpoint.addAction( _action ); }

		  private:
			Model::Viewpoint & _viewpoint;
			const std::string  _action;
		};
	} // namespace Action
} // namespace VTX
#endif
