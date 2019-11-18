#include "base_component.hpp"
#include <iostream>

namespace VTX
{
	namespace UI
	{
		BaseComponent::BaseComponent( bool * const p_show ) : _show( p_show ) {}

		BaseComponent::~BaseComponent()
		{
			for ( BaseComponent * const component : _components )
			{
				delete component;
			}
			_components.clear();
		}

		void BaseComponent::init()
		{
			if ( _isInitialized ) { return; }
			_addComponents();
			_registerEventHandlers();
			_isInitialized = true;
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

		void BaseComponent::_registerEventHandler( const Event::EVENT_UI p_event ) { _events.emplace( p_event ); }

		void BaseComponent::receiveEvent( const Event::EVENT_UI p_event, void * p_arg )
		{
			if ( std::find( _events.begin(), _events.end(), p_event ) != _events.end() )
			{ _applyEvent( p_event, p_arg ); }

			// Propagate to children.
			for ( BaseComponent * const component : _components )
			{
				// Only shown components?
				if ( component->isShown() == false ) { continue; }
				component->receiveEvent( p_event, p_arg );
			}
		}

		void BaseComponent::display() { _draw(); };

	} // namespace UI
} // namespace VTX
