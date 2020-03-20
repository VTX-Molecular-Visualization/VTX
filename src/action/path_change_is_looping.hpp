#ifndef __VTX_ACTION_PATH_CHANGE_IS_LOOPING__
#define __VTX_ACTION_PATH_CHANGE_IS_LOOPING__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/path.hpp"

namespace VTX
{
	namespace Action
	{
		class PathChangeIsLooping : public BaseAction
		{
		  public:
			explicit PathChangeIsLooping( Model::Path & p_path, const bool p_isLooping ) :
				_path( p_path ), _isLooping( p_isLooping )
			{
			}

			virtual void execute() override { _path.setIsLooping( _isLooping ); }

		  private:
			Model::Path & _path;
			const bool	  _isLooping;
		};
	} // namespace Action
} // namespace VTX
#endif
