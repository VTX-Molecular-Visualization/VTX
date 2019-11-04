#include "base_component.hpp"
#include "../util/logger.hpp"
#include <iostream>

namespace VTX
{
	namespace UI
	{
		BaseComponent::BaseComponent( bool * p_show ) : _show( p_show ) {}

		void BaseComponent::init()
		{
			if ( _isInitialized ) { return; }
			_addComponents();
			_isInitialized = true;
		}

		BaseComponent::~BaseComponent()
		{
			for ( BaseComponent * component : _components )
			{
				delete component;
			}
			_components.clear();
		}

		void BaseComponent::_addComponent( BaseComponent * p_component )
		{
			p_component->init();
			_components.push_back( p_component );
		}

		void BaseComponent::_drawComponents()
		{
			for ( BaseComponent * component : _components )
			{
				if ( component->isShown() == false ) { continue; }
				component->display();
			}
		}

		void BaseComponent::_registerEventHandler(
			Event::EVENT_UI				   p_event,
			std::function<void( va_list )> p_callback )
		{
			try
			{
				_events.try_emplace( p_event, p_callback );
			}
			catch ( std::exception p_e )
			{
				WRG( "Event UI already registered" );
			}
		}

		void BaseComponent::receiveEvent( Event::EVENT_UI p_event,
										  va_list		  p_args )
		{
			try
			{
				_events.at( p_event )( p_args );
			}
			catch ( std::exception )
			{
				// Nothing.
			}

			// Propagate to children.
			for ( BaseComponent * component : _components )
			{
				if ( component->isShown() == false ) { continue; }
				component->receiveEvent( p_event, p_args );
			}
		}

		void BaseComponent::display() { _draw(); };

	} // namespace UI
} // namespace VTX
