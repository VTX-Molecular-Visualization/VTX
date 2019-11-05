#ifndef __EVENT__
#define __EVENT__

namespace VTX
{
	namespace Event
	{
		enum class EVENT_UI
		{
			LOG_CONSOLE
		};

		template<typename T>
		class Event
		{
		  public:
			Event( T p_arg ) : _data( p_arg ) {};
			T data() { return _data; }

		  private:
			T _data;
		};

	} // namespace Event
} // namespace VTX
#endif
