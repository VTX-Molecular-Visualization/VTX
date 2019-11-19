#ifndef __VTX_BASE_COMPONENT__
#define __VTX_BASE_COMPONENT__

#include "../defines.hpp"
#include "../event/event.hpp"
#include "../lib/magic_enum.hpp"
#include "enum_component_type.hpp"
#include <cstdarg>
#include <functional>
#include <map>
#include <set>

namespace VTX
{
	namespace UI
	{
		class BaseComponent
		{
		  public:
			BaseComponent( bool * const );
			~BaseComponent();
			virtual void display() final;
			virtual void init();
			bool		 isShown() { return *_show; }

			void receiveEvent( const Event::EVENT_UI, void * const );

			virtual COMPONENT_TYPE getName() const = 0;

		  protected:
			bool * _show;

			virtual void _addComponent( BaseComponent * const ) final;
			virtual void _addComponents() {};
			virtual void _drawComponents() final;

			virtual void _registerEventHandler( const Event::EVENT_UI ) final;
			virtual void _registerEventHandlers() {};
			virtual void _applyEvent( const Event::EVENT_UI, void * p_arg ) {};

			virtual void _draw() = 0;

		  private:
			bool									  _isInitialized = false;
			std::map<COMPONENT_TYPE, BaseComponent *> _components	 = std::map<COMPONENT_TYPE, BaseComponent *>();
			std::set<Event::EVENT_UI>				  _events		 = std::set<Event::EVENT_UI>();
		};
	} // namespace UI
} // namespace VTX
#endif
