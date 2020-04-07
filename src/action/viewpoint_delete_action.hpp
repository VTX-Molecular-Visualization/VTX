#ifndef __VTX_ACTION_VIEWPOINT_DELETE_ACTION__
#define __VTX_ACTION_VIEWPOINT_DELETE_ACTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/viewpoint.hpp"

namespace VTX
{
	namespace Action
	{
		class ViewpointDeleteAction : public BaseAction
		{
		  public:
			explicit ViewpointDeleteAction( Model::Viewpoint &								 p_viewpoint,
											const std::vector<std::string>::const_iterator & p_action ) :
				_viewpoint( p_viewpoint ),
				_action( p_action )
			{
			}

			virtual void execute() override { _viewpoint.removeAction( _action ); }

		  private:
			Model::Viewpoint &							   _viewpoint;
			const std::vector<std::string>::const_iterator _action;
		};
	} // namespace Action
} // namespace VTX
#endif
