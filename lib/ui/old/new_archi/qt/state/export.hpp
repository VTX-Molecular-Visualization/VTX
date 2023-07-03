#ifndef __VTX_UI_QT_STATE_EXPORT__
#define __VTX_UI_QT_STATE_EXPORT__

#include "base_state.hpp"
#include <app/old/component/video/_fwd.hpp>
#include <util/types.hpp>

namespace VTX::UI::QT::State
{
	class Export : public BaseState
	{
	  public:
		Export() = default;

		virtual void enter( void * const ) override;
		virtual void exit() override;
		virtual void update( const float & ) override;

	  private:
		std::string							   _directoryName;
		const App::Old::Component::Video::Path * _path;
		const std::vector<std::string> *	   _actions	   = nullptr;
		uint								   _frame	   = 0u;
		uint								   _frameCount = 0u;

		// void _generareVideo() const;
	};
} // namespace VTX::UI::QT::State
#endif
