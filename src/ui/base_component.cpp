#include "base_component.hpp"
#include "../util/logger.hpp"
#include <iostream>

namespace VTX
{
	namespace UI
	{
		BaseComponent::BaseComponent() {}

		void BaseComponent::init() { _addComponents(); }

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
			p_component->init();
			_components.push_back( p_component );
		}

		void BaseComponent::_drawComponents()
		{
			for ( BaseComponent * component : _components )
			{
				component->display();
			}
		}

		void BaseComponent::display() { _draw(); };

	} // namespace UI
} // namespace VTX
