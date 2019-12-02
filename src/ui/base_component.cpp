#include "base_component.hpp"
#include <iostream>

namespace VTX
{
	namespace UI
	{
		BaseComponent::BaseComponent( bool * const p_show ) : _show( p_show ) {}

		BaseComponent::~BaseComponent()
		{
			for ( auto & [ name, component ] : _components )
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

		void BaseComponent::_addComponent( const std::shared_ptr<BaseComponent> p_component )
		{
			p_component->init();
			try
			{
				_components.try_emplace( p_component->getName(), p_component );
			}
			catch ( const std::exception )
			{
				VTX_WARNING( "A view with this type already exists: "
							 + std::string( magic_enum::enum_name( p_component->getName() ) ) );
			}
		}

		void BaseComponent::_drawComponent( const COMPONENT_NAME p_name )
		{
			try
			{
				_components.at( p_name )->display();
			}
			catch ( const std::exception )
			{
				VTX_WARNING( "Component not found" );
			}
		}

		void BaseComponent::_drawComponents()
		{
			for ( const auto & [ type, component ] : _components )
			{
				component->display();
			}
		}

		void BaseComponent::_registerEventHandler( const Event::EVENT_UI p_event ) { _events.emplace( p_event ); }

		const std::shared_ptr<BaseComponent> BaseComponent::getComponentByName( const COMPONENT_NAME p_name ) const
		{
			try
			{
				return _components.at( p_name );
			}
			catch ( const std::exception )
			{
				for ( const auto & [ name, component ] : _components )
				{
					std::shared_ptr<BaseComponent> child = component->getComponentByName( p_name );
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
			for ( auto const & [ name, component ] : _components )
			{
				component->receiveEvent( p_event, p_arg );
			}
		}

		void BaseComponent::display()
		{
			if ( _show != nullptr && isShown() == false ) { return; }
			_draw();
		};

	} // namespace UI
} // namespace VTX
