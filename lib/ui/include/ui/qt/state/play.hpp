#ifndef __VTX_UI_QT_STATE_PLAY__
#define __VTX_UI_QT_STATE_PLAY__

#include "base_state.hpp"
#include <app/old/component/video/_fwd.hpp>

namespace VTX::UI::QT::State
{
	class Play : public BaseState
	{
	  public:
		Play() = default;

		virtual void enter( void * const ) override;
		virtual void exit() override;
		virtual void update( const float & ) override;

	  private:
		const App::Old::Component::Video::Path * _path	= nullptr;
		const std::vector<std::string> *	   _actions = nullptr;
		float								   _time	= 0.f;

		void _setCamera() const;
		void _executeActions( const float p_time );
	};
} // namespace VTX::UI::QT::State
#endif
