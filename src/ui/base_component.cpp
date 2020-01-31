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
			if ( _hasItem( p_name ) ) { _getItem( p_name )->draw(); }
		}

		void BaseComponent::_drawComponents()
		{
			for ( const PairStringToItemPtr pair : _getItems() )
			{
				pair.second->draw();
			}
		}

		void BaseComponent::_registerEventHandler( const Event::EVENT_UI p_event ) { _events.emplace( p_event ); }

		BaseComponent * const BaseComponent::getComponentByName( const std::string & p_name )
		{
			if ( _getItems().find( p_name ) != _getItems().end() ) { return (BaseComponent *)_getItem( p_name ); }

			for ( const PairStringToItemPtr pair : _getItems() )
			{
				BaseComponent * child = dynamic_cast<BaseComponent *>( pair.second );
				if ( child != nullptr )
				{
					child = child->getComponentByName( p_name );
					if ( child != nullptr ) { return child; }
				}
			}

			return nullptr;
		}

		void BaseComponent::receiveEvent( const Event::EVENT_UI p_event, void * const p_arg )
		{
			if ( std::find( _events.begin(), _events.end(), p_event ) != _events.end() )
			{ _applyEvent( p_event, p_arg ); }

			// Propagate to children.
			for ( const PairStringToItemPtr pair : _getItems() )
			{
				BaseComponent * child = dynamic_cast<BaseComponent *>( pair.second );
				if ( child != nullptr ) { child->receiveEvent( p_event, p_arg ); }
			}
		}

	} // namespace UI
} // namespace VTX
