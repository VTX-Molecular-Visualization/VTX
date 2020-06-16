#include "base_component.hpp"
#include "model/molecule.hpp"
#include "vtx_app.hpp"

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

		void BaseComponent::_draw()
		{
			if ( _drawHeader() == false )
			{
				_drawFooter();
				return;
			}

			_drawContent();

			// Handle focus.
			if ( ImGui::IsWindowFocused() && VTXApp::get().getUI().getFocusedWindow() != getName() )
			{
				VTXApp::get().getUI().setFocusedWindow( getName() );
			}
			if ( ImGui::IsWindowHovered() && VTXApp::get().getUI().getHoveredWindow() != getName() )
			{
				VTXApp::get().getUI().setHoveredWindow( getName() );
			}

			_drawFooter();
		}

		void BaseComponent::_drawContent() { _drawComponents(); }

		void BaseComponent::_drawComponent( const std::string & p_name )
		{
			if ( hasItem( p_name ) )
			{
				getItem( p_name )->draw();
			}
			else
			{
				VTX_DEBUG( "Unknown component: " + p_name );
			}
		}

		void BaseComponent::_drawComponents()
		{
			for ( const std::string & key : _getOrderedKeys() )
			{
				getItem( key )->draw();
			}
		}

		BaseComponent * const BaseComponent::getComponentByName( const std::string & p_name )
		{
			if ( hasItem( p_name ) )
			{
				return static_cast<BaseComponent *>( getItem( p_name ) );
			}

			for ( const PairStringToItemPtr & pair : _getItems() )
			{
				BaseComponent * child = dynamic_cast<BaseComponent *>( pair.second );
				if ( child != nullptr )
				{
					child = child->getComponentByName( p_name );
					if ( child != nullptr )
					{
						return child;
					}
				}
			}

			return nullptr;
		}

	} // namespace UI
} // namespace VTX
