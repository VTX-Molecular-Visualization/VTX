#ifndef __VTX_ACTION_PATH_CHANGE_INTERPOLATION_MODE__
#define __VTX_ACTION_PATH_CHANGE_INTERPOLATION_MODE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/path.hpp"

namespace VTX
{
	namespace Action
	{
		class PathChangeInterpolationMode : public BaseAction
		{
		  public:
			explicit PathChangeInterpolationMode( Model::Path &							 p_path,
												  const Model::Path::INNTERPOLATION_MODE p_mode ) :
				_path( p_path ),
				_mode( p_mode )
			{
			}

			virtual void execute() override
			{
				_path.setInterpolationMode( _mode );
				_path.refreshAllDurations();
			}

		  private:
			Model::Path &						   _path;
			const Model::Path::INNTERPOLATION_MODE _mode;
		};
	} // namespace Action
} // namespace VTX
#endif
