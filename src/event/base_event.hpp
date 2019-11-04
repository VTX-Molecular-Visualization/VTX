#ifndef __BASE_EVENT__
#define __BASE_EVENT__

namespace VTX
{
	namespace Event
	{
		template<typename T>
		class BaseEvent
		{
		  public:
			BaseEvent( T );

			T getData() { return _data; }

		  private:
			const T _data final;
		};
	} // namespace Event
} // namespace VTX
#endif
