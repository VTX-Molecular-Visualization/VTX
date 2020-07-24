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
			Visualization() = default;

			virtual const std::string & getName() const override { return ID::State::VISUALIZATION; }
			virtual void				enter( void * const ) override;
			virtual void				exit() override;

			const ID::VTX_ID & getController() const { return _controller; }

			virtual void update( const double & p_deltaTime ) override;

			void									 toggleController();
			void									 recenter();
			const Controller::BaseController * const getCurrentController() const { return getItemAt( _controller ); }
			Controller::BaseController * const		 getCurrentController() { return getItem( _controller ); }

			virtual void receiveEvent( const Event::VTXEvent & p_event ) override;

		  protected:
			virtual std::vector<Event::VTX_EVENT> _getEvents() const override
			{
				return std::vector<Event::VTX_EVENT>( 1, Event::Global::ON_SCENE_CHANGE );
			}

		  private:
			ID::VTX_ID _controller = ID::Controller::FREEFLY;
		};
	} // namespace State
} // namespace VTX
#endif
