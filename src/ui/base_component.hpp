#ifndef __VTX_BASE_COMPONENT__
#define __VTX_BASE_COMPONENT__

#include "../event/event.hpp"
#include "../util/logger.hpp"
#include <cstdarg>
#include <functional>
#include <set>
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

			void receiveEvent( const Event::EVENT_UI, void * const );

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
			bool						 _isInitialized = false;
			std::vector<BaseComponent *> _components	= std::vector<BaseComponent *>();
			std::set<Event::EVENT_UI>	 _events		= std::set<Event::EVENT_UI>();
		};
	} // namespace UI
} // namespace VTX
#endif
