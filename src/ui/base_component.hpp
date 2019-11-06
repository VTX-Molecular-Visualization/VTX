#ifndef __VTX_BASE_COMPONENT__
#define __VTX_BASE_COMPONENT__

#include "../event/event.hpp"
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
			BaseComponent( bool * const );
			~BaseComponent();
			virtual void display() final;
			virtual void init();
			bool		 isShown() { return *_show; }

			template<typename T>
			void receiveEvent( const Event::EVENT_UI, const T & );

		  protected:
			bool * _show;

			virtual void _addComponent( BaseComponent * const ) final;
			virtual void _addComponents() {};
			virtual void _drawComponents() final;

			template<typename T>
			void		 _registerEventHandler( const Event::EVENT_UI,
												const std::function<void( T )> );
			virtual void _registerEventHandlers() {};

			virtual void _draw() = 0;

		  private:
			bool						 _isInitialized = false;
			std::vector<BaseComponent *> _components
				= std::vector<BaseComponent *>();
			std::map<Event::EVENT_UI, std::function<void( const void * )>>
				_events = std::map<Event::EVENT_UI,
								   std::function<void( const void * )>>();
		};
	} // namespace UI
} // namespace VTX
#endif
