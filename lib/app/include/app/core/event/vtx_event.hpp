#ifndef __VTX_APP_CORE_EVENT_VTX_EVENT__
#define __VTX_APP_CORE_EVENT_VTX_EVENT__

#include <string>
#include <tuple>

namespace VTX::App::Core::Event
{
	// Global system events [n-n].
	using VTX_EVENT = int;

	// Base event without args.
	struct VTXEvent
	{
		VTXEvent( const VTX_EVENT & p_event ) : name( p_event ) {}
		virtual ~VTXEvent() = default;
		const VTX_EVENT name;
	};

	// Event with args.
	template<typename... Args>
	struct VTXEventArg : public VTXEvent
	{
	  public:
		VTXEventArg( const VTX_EVENT & p_event, Args... p_args ) : VTXEvent( p_event ), _args( p_args... ) {}
		virtual ~VTXEventArg() = default;

		const typename std::tuple_element<0, std::tuple<Args...>>::type & get() const { return get<0>(); }

		template<std::size_t I>
		const typename std::tuple_element<I, std::tuple<Args...>>::type & get() const
		{
			return std::get<I>( _args );
		}

	  private:
		std::tuple<Args...> _args;
	};

} // namespace VTX::App::Core::Event
#endif
