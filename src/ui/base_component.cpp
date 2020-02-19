#include "base_component.hpp"
#include "model/molecule.hpp"

namespace VTX
{
	namespace UI
	{
		void BaseComponent::init()
		{
			Generic::HasCollection<Generic::BaseDrawable>::init();
			Event::BaseEventReceiverVTX::_registerEvents();
			_visible = true;
		}

		void BaseComponent::clean()
		{
			_visible = false;
			Event::BaseEventReceiverVTX::_unregisterEvents();
			Generic::HasCollection<Generic::BaseDrawable>::clean();
		}

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

		BaseComponent * const BaseComponent::getComponentByName( const std::string & p_name )
		{
			if ( _getItems().find( p_name ) != _getItems().end() )
			{ return static_cast<BaseComponent *>( _getItem( p_name ) ); }

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

	} // namespace UI
} // namespace VTX
