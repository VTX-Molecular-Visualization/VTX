#ifndef __VTX_STATE_VISUALIZATION__
#define __VTX_STATE_VISUALIZATION__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_state.hpp"

namespace VTX
{
	namespace State
	{
		class Visualization : public BaseState
		{
		  public:
			Visualization();

			virtual void enter( void * const ) override;
			virtual void exit() override;

			const ID::VTX_ID & getController() const { return _controller; }

			virtual void update( const double & p_deltaTime ) override;

			void									 toggleController();
			void									 setController( const ID::VTX_ID & p_controllerId );
			void									 recenter();
			const Controller::BaseController * const getCurrentController() const { return getItemAt( _controller ); }
			Controller::BaseController * const		 getCurrentController() { return getItem( _controller ); }
			const ID::VTX_ID &						 getCurrentControllerID() const { return _controller; }

			virtual void receiveEvent( const Event::VTXEvent & p_event ) override;

		  private:
			ID::VTX_ID _controller = ID::Controller::TRACKBALL;
		};
	} // namespace State
} // namespace VTX
#endif
