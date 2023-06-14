#ifndef __VTX_UI_QT_BASE_STATE__
#define __VTX_UI_QT_BASE_STATE__

#include "ui/core/controller/base_controller.hpp"
#include <app/old/application/generic/base_updatable.hpp>
#include <app/old/core/event/base_event_receiver_vtx.hpp>
#include <app/old/id.hpp>
#include <map>

namespace VTX::UI::QT::State
{
	class BaseState :
		public App::Old::Application::Generic::BaseUpdatable,
		public VTX::App::Old::Core::Event::BaseEventReceiverVTX
	{
	  public:
		BaseState() = default;
		virtual ~BaseState()
		{
			for ( const std::pair<const App::Old::VTX_ID, Core::Controller::BaseController * const> & pair : _controllers )
			{
				delete pair.second;
			}
			_controllers.clear();
		}

		template<typename T, typename = std::enable_if<std::is_base_of<Core::Controller::BaseController, T>::value>>
		inline T * const getController( const App::Old::VTX_ID & p_id )
		{
			return dynamic_cast<T * const>( _controllers[ p_id ] );
		}
		template<typename T, typename = std::enable_if<std::is_base_of<Core::Controller::BaseController, T>::value>>
		inline const T * const getController( const App::Old::VTX_ID & p_id ) const
		{
			return dynamic_cast<T * const>( _controllers.at( p_id ) );
		}

		virtual void enter( void * const )
		{
			for ( const std::pair<const App::Old::VTX_ID, Core::Controller::BaseController * const> & pair : _controllers )
			{
				pair.second->setActive( true );
			}
		}

		virtual void exit()
		{
			for ( const std::pair<const App::Old::VTX_ID, Core::Controller::BaseController * const> & pair : _controllers )
			{
				pair.second->setActive( false );
			}
		}

		virtual void update( const float & p_deltaTime ) override
		{
			for ( const std::pair<const App::Old::VTX_ID, Core::Controller::BaseController * const> & pair : _controllers )
			{
				if ( pair.second->isActive() )
				{
					pair.second->update( p_deltaTime );
				}
			}
		}

	  protected:
		std::map<const App::Old::VTX_ID, Core::Controller::BaseController * const> _controllers
			= std::map<const App::Old::VTX_ID, Core::Controller::BaseController * const>();
	};
} // namespace VTX::UI::QT::State
#endif
