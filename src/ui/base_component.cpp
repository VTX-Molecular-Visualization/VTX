#include "base_component.hpp"
#include <iostream>

namespace VTX
{
	namespace UI
	{
		BaseComponent::BaseComponent() { _addComponents(); }

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
			_components.push_back( p_component );
		}

		void BaseComponent::display()
		{
			_draw();
			for ( BaseComponent * component : _components )
			{
				component->display();
			}
		};
	} // namespace UI
} // namespace VTX
