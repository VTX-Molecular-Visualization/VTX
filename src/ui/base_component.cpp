#include "base_component.hpp"
#include <iostream>

namespace VTX
{
	namespace UI
	{
		BaseComponent::BaseComponent() { _addComponents(); }

		BaseComponent::~BaseComponent() {}

		void BaseComponent::_addComponent( BaseComponent * p_component )
		{
			_components.push_back( p_component );
		}

		void BaseComponent::display()
		{
			_draw();
			for ( int i = 0; i < _components.size(); ++i )
			{
				//_components[ i ].display();
			}
		};
	} // namespace UI
} // namespace VTX
