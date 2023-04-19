#ifndef __VTX_UI_QT_STATE_EXPORT__
#define __VTX_UI_QT_STATE_EXPORT__

#include "base_state.hpp"
#include <util/types.hpp>

namespace VTX
{
	namespace Model
	{
		class Path;
	}

	namespace UI::QT::State
	{
		class Export : public BaseState
		{
		  public:
			Export() = default;

			virtual void enter( void * const ) override;
			virtual void exit() override;
			virtual void update( const float & ) override;

		  private:
			std::string						 _directoryName;
			const Model::Path *				 _path;
			const std::vector<std::string> * _actions	 = nullptr;
			uint							 _frame		 = 0u;
			uint							 _frameCount = 0u;

			// void _generareVideo() const;
		};
	} // namespace UI::QT::State
} // namespace VTX
#endif
