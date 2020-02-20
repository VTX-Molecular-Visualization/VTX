#ifndef __VTX_ACTION_VIEWPOINT_DELETE__
#define __VTX_ACTION_VIEWPOINT_DELETE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/path.hpp"
#include "model/viewpoint.hpp"

namespace VTX
{
	namespace Action
	{
		class ViewpointDelete : public BaseAction
		{
		  public:
			explicit ViewpointDelete( Model::Viewpoint & p_viewpoint ) : _viewpoint( p_viewpoint ) {}

			virtual void execute() override
			{
				_viewpoint.getPathPtr()->resetSelectedViewpoint();
				_viewpoint.getPathPtr()->removeViewpoint( &_viewpoint );
				Generic::destroy<Model::Viewpoint>( &_viewpoint );
				_viewpoint.getPathPtr()->refreshAllDurations();
			}

		  private:
			Model::Viewpoint & _viewpoint;
		};
	} // namespace Action
} // namespace VTX
#endif
