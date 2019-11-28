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
			virtual ~BaseComponent();

			virtual void display();
			virtual void init();
			virtual bool isShown() final { return *_show; }

			virtual const std::shared_ptr<BaseComponent> getComponentByType( const COMPONENT_TYPE ) const final;
			virtual void								 receiveEvent( const Event::EVENT_UI, void * const ) final;

		  protected:
			bool * _show = nullptr;

			virtual void _addComponent( const COMPONENT_TYPE, const std::shared_ptr<BaseComponent> ) final;
			virtual void _addComponents() {};
			virtual void _drawComponent( const COMPONENT_TYPE );
			virtual void _drawComponents() final;

			virtual void _registerEventHandler( const Event::EVENT_UI ) final;
			virtual void _registerEventHandlers() {};
			virtual void _applyEvent( const Event::EVENT_UI, void * const p_arg ) {};

			virtual void _draw() = 0;

		  private:
			bool													 _isInitialized = false;
			std::map<COMPONENT_TYPE, std::shared_ptr<BaseComponent>> _components
				= std::map<COMPONENT_TYPE, std::shared_ptr<BaseComponent>>();
			std::set<Event::EVENT_UI> _events = std::set<Event::EVENT_UI>();
		};
	} // namespace UI
} // namespace VTX
#endif
