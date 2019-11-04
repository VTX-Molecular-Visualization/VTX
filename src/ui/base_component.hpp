#ifndef __BASE_COMPONENT__
#define __BASE_COMPONENT__

#include "../event/event_ui.hpp"
#include "../util/logger.hpp"
#include <cstdarg>
#include <functional>
#include <map>
//#include <type_traits>
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
			void		 receiveEvent( const Event::EVENT_UI, const int, ... );

		  protected:
			bool * _show;

			virtual void _addComponent( BaseComponent * const ) final;
			virtual void _addComponents() {};
			virtual void _drawComponents() final;

			virtual void _registerEventHandler(
				const Event::EVENT_UI,
				const std::function<void( const int, ... )> ) final;
			virtual void _registerEventHandlers() {};

			virtual void _draw() = 0;

		  private:
			bool						 _isInitialized = false;
			std::vector<BaseComponent *> _components
				= std::vector<BaseComponent *>();
			std::map<Event::EVENT_UI, std::function<void( const int, ... )>>
				_events = std::map<Event::EVENT_UI,
								   std::function<void( const int, ... )>>();
		};
	} // namespace UI
} // namespace VTX
#endif
