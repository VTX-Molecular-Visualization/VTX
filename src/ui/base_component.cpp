#include "base_component.hpp"
#include "../model/model_molecule.hpp"
#include "../util/type.hpp"

namespace VTX
{
	namespace UI
	{
		void BaseComponent::init()
		{
			Generic::HasCollection<Generic::BaseDrawable>::init();
			_registerEventHandlers();
		}

		void BaseComponent::initItem() { init(); }

		void BaseComponent::_drawComponent( const std::string & p_name )
		{
			try
			{
				_getItem( p_name )->draw();
			}
			catch ( const std::exception & )
			{
				VTX_WARNING( "Component not found: " + p_name );
			}
		}

		void BaseComponent::_drawComponents()
		{
			for ( const PairStringToItemPtr pair : _getItems() )
			{
				pair.second->draw();
			}
		}

		void BaseComponent::_registerEventHandler( const Event::EVENT_UI p_event ) { _events.emplace( p_event ); }

		/*
		const UI::BaseComponent * const BaseComponent::getComponentByName( const std::string & p_name ) const
		{
			if ( _components.find( p_name ) != _components.end() ) { return _components.at( p_name ); }

			for ( const PairStringToComponentSharedPtr pair : _components )
			{
				std::shared_ptr<UI::BaseComponent> child = pair.second->getComponentByName( p_name );
				if ( child != nullptr ) { return child; }
			}

			return nullptr;
		}
		*/

		void BaseComponent::receiveEvent( const Event::EVENT_UI p_event, void * const p_arg )
		{
			if ( std::find( _events.begin(), _events.end(), p_event ) != _events.end() )
			{ _applyEvent( p_event, p_arg ); }

			// Propagate to children.
			for ( const PairStringToItemPtr pair : _getItems() )
			{
				// pair.second->receiveEvent( p_event, p_arg );
			}
		}

	} // namespace UI
} // namespace VTX
