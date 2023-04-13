#ifndef __VTX_BASE_NOTIFIABLE__
#define __VTX_BASE_NOTIFIABLE__

namespace VTX
{
	namespace Generic
	{
		template<typename T>
		class BaseNotifiable
		{
		  public:
			virtual ~BaseNotifiable() = default;

			virtual void notify( const T & p_eventType ) = 0;
		};
	} // namespace Generic
} // namespace VTX
#endif
