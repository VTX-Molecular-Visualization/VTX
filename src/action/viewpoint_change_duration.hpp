#ifndef __VTX_ACTION_VIEWPOINT_CHANGE_DURATION__
#define __VTX_ACTION_VIEWPOINT_CHANGE_DURATION__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/viewpoint.hpp"

namespace VTX
{
	namespace Action
	{
		class ViewpointChangeDuration : public BaseAction
		{
		  public:
			explicit ViewpointChangeDuration( Model::Viewpoint & p_viewpoint, const float p_duration ) :
				_viewpoint( p_viewpoint ), _duration( p_duration )
			{
			}

			virtual void execute() override
			{
				_viewpoint.setDuration( Util::Math::max<float>( _duration, 0.f ) );
				_viewpoint.getPathPtr()->refreshAllDurations();
			}

		  private:
			Model::Viewpoint & _viewpoint;
			const float		   _duration;
		};
	} // namespace Action
} // namespace VTX
#endif
