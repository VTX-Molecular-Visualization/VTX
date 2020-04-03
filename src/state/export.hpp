#ifndef __VTX_STATE_EXPORT__
#define __VTX_STATE_EXPORT__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_state.hpp"
#include "model/path.hpp"

namespace VTX
{
	namespace State
	{
		class Export : public BaseState
		{
		  public:
			Export() = default;

			virtual const std::string & getName() const override { return ID::State::EXPORT; }
			virtual void				enter( void * const ) override;
			virtual void				exit() override;
			virtual void				update( const double ) override;

		  private:
			Model::Path *					 _path		 = nullptr;
			const std::vector<std::string> * _actions	 = nullptr;
			uint							 _frame		 = 0u;
			uint							 _frameCount = 0u;
		};
	} // namespace State
} // namespace VTX
#endif
