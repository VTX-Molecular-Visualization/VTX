#ifndef __VTX_BASE_COMPONENT__
#define __VTX_BASE_COMPONENT__

#include "../event/event.hpp"
#include "../util/logger.hpp"
#include <cstdarg>
#include <functional>
#include <map>
#include <vector>

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

			virtual void receiveEvent( const Event::EVENT_UI, const int p_args... ) final;

		  protected:
			bool * _show;

			virtual void _addComponent( BaseComponent * const ) final;
			virtual void _addComponents() {};
			virtual void _drawComponents() final;

			virtual void _registerEventHandler( const Event::EVENT_UI ) final;
			virtual void _registerEventHandlers() {};
			virtual void _applyEvent( const Event::EVENT_UI, const int p_args... ) {};

			virtual void _draw() = 0;

		  private:
			bool						 _isInitialized = false;
			std::vector<BaseComponent *> _components	= std::vector<BaseComponent *>();
			// TODO: replace by std::set
			std::vector<Event::EVENT_UI> _events = std::vector<Event::EVENT_UI>();
		};
	} // namespace UI
} // namespace VTX
#endif
