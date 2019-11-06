#include "base_component.hpp"
#include <iostream>

namespace VTX
{
	namespace UI
	{
		BaseComponent::BaseComponent( bool * const p_show ) : _show( p_show ) {}

		void BaseComponent::init()
		{
			if ( _isInitialized ) { return; }
			_addComponents();
			_registerEventHandlers();
			_isInitialized = true;
		}

		BaseComponent::~BaseComponent()
		{
			for ( BaseComponent * const component : _components )
			{
				delete component;
			}
			_components.clear();
		}

		void BaseComponent::_addComponent( BaseComponent * const p_component )
		{
			p_component->init();
			_components.push_back( p_component );
		}

		void BaseComponent::_drawComponents()
		{
			for ( BaseComponent * const component : _components )
			{
				if ( component->isShown() == false ) { continue; }
				component->display();
			}
		}

		template<typename T>
		void BaseComponent::_registerEventHandler(
			const Event::EVENT_UI				 p_event,
			const std::function<void( const T )> p_callback )
		{
			try
			{
				_events.try_emplace( p_event, p_callback );
			}
			catch ( const std::exception p_e )
			{
				WRG( "Event UI already registered" );
			}
		}

		template<typename T>
		void BaseComponent::receiveEvent( const Event::EVENT_UI p_name,
										  const T &				p_arg )
		{
			try
			{
				_events.at( p_name )( p_arg );
			}
			catch ( const std::exception )
			{
				// Nothing.
			}

			// Propagate to children.
			for ( BaseComponent * const component : _components )
			{
				// Only shown components?
				if ( component->isShown() == false ) { continue; }
				component->receiveEvent<T>( p_name, p_event );
			}
		}

		void BaseComponent::display() { _draw(); };

	} // namespace UI
} // namespace VTX
