#ifndef __UI_EVENT__
#define __UI_EVENT__

#include "base_event.hpp"

namespace VTX
{
	namespace Event
	{
		template<typename T>
		class UIEvent : public BaseEvent<T>
		{
		  public:
			enum EVENT_UI
			{
				EVENT_UI_LOG_CONSOLE
			};

			EVENT_UI getName() { return _name; }

		  private:
			EVENT_UI _name;
		};
	} // namespace Event
} // namespace VTX
#endif
