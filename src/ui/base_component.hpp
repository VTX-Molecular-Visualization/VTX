#ifndef __BASE_COMPONENT__
#define __BASE_COMPONENT__

#include "../event/event_ui.hpp"
#include "../util/logger.hpp"
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
			BaseComponent( bool * );
			~BaseComponent();
			virtual void display() final;
			virtual void init();
			bool		 isShown() { return *_show; }
			void		 receiveEvent( Event::UIEvent<> & );

		  protected:
			bool * _show;

			virtual void _addComponent( BaseComponent * ) final;
			virtual void _addComponents() {};
			virtual void _drawComponents() final;

			virtual void _registerEventHandler(
				Event::UIEvent::EVENT_UI,
				std::function<void( ... )> ) final;
			virtual void _registerEventHandlers() {};

			virtual void _draw() = 0;

		  private:
			bool						 _isInitialized = false;
			std::vector<BaseComponent *> _components
				= std::vector<BaseComponent *>();
			std::map<Event::UIEvent::EVENT_UI, std::function<void( ... )>>
				_events = std::map<Event::UIEvent::EVENT_UI,
								   std::function<void( ... )>>();
		};
	} // namespace UI
} // namespace VTX
#endif
