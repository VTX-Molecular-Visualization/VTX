#ifndef __VTX_ACTION_PATH_CHANGE_DURATION__
#define __VTX_ACTION_PATH_CHANGE_DURATION__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/path.hpp"

namespace VTX
{
	namespace Action
	{
		class PathChangeDuration : public BaseAction
		{
		  public:
			explicit PathChangeDuration( Model::Path & p_path, const float p_duration ) :
				_path( p_path ), _duration( p_duration )
			{
			}

			virtual void execute() override
			{
				_path.setDuration( glm::max<float>( _duration, 0.f ) );
				_path.refreshAllDurations();
			}

		  private:
			Model::Path & _path;
			const float	  _duration;
		};
	} // namespace Action
} // namespace VTX
#endif
