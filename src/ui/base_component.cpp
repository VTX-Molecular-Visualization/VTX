#include "base_component.hpp"
#include <iostream>

namespace VTX
{
	namespace UI
	{
		BaseComponent::BaseComponent( bool * const p_show ) : _show( p_show ) {}

		BaseComponent::~BaseComponent()
		{
			for ( auto & [ type, component ] : _components )
			{
				component.reset();
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

		void BaseComponent::_addComponent( const COMPONENT_TYPE					p_type,
										   const std::shared_ptr<BaseComponent> p_component )
		{
			p_component->init();
			try
			{
				_components.try_emplace( p_type, p_component );
			}
			catch ( const std::exception )
			{
				VTX_WARNING( "A view with this type already exists: "
							 + std::string( magic_enum::enum_name( p_type ) ) );
			}
		}

		void BaseComponent::_drawComponents()
		{
			for ( const auto & [ type, component ] : _components )
			{
				if ( component->isShown() == false ) { continue; }
				component->display();
			}
		}

		void BaseComponent::_registerEventHandler( const Event::EVENT_UI p_event ) { _events.emplace( p_event ); }

		const std::shared_ptr<BaseComponent> BaseComponent::getComponentByType( const COMPONENT_TYPE p_type ) const
		{
			try
			{
				return _components.at( p_type );
			}
			catch ( const std::exception )
			{
				for ( const auto & [ type, component ] : _components )
				{
					std::shared_ptr<BaseComponent> child = component->getComponentByType( p_type );
					if ( child != nullptr ) { return child; }
				}

				return nullptr;
			}
		}

		void BaseComponent::receiveEvent( const Event::EVENT_UI p_event, void * const p_arg )
		{
			if ( std::find( _events.begin(), _events.end(), p_event ) != _events.end() )
			{ _applyEvent( p_event, p_arg ); }

			// Propagate to children.
			for ( auto const & [ type, component ] : _components )
			{
				// Only shown components?
				if ( component->isShown() == false )
				{ /*continue;*/
				}
				component->receiveEvent( p_event, p_arg );
			}
		}

		void BaseComponent::display() { _draw(); };

	} // namespace UI
} // namespace VTX
