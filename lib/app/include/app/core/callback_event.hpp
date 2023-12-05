#ifndef __VTX_APP_CORE_CALLBACK_EVENT__
#define __VTX_APP_CORE_CALLBACK_EVENT__

#include <functional>
#include <map>

namespace VTX::App::Core
{
	template<typename... Args>
	class CallbackRegister
	{
	  protected:
		using callback_t = std::function<void( const Args... )>;

	  public:
		CallbackRegister() = default;

		void addCallback( void * const p_listener, const callback_t & p_callback )
		{
			_mapCallbacks[ p_listener ] = p_callback;
		}
		void removeCallback( void * const p_listener ) { _mapCallbacks.erase( p_listener ); }
		void clear() { _mapCallbacks.clear(); }

	  protected:
		const std::map<void *, callback_t> & getCallbacks() const { return _mapCallbacks; }

	  private:
		std::map<void *, callback_t> _mapCallbacks = std::map<void *, callback_t>();
	};

	template<typename... Args>
	class CallbackEmitter : public CallbackRegister<Args...>
	{
	  public:
		CallbackEmitter() = default;

		void emit( const Args... p_params ) const
		{
			for ( auto pairListenerCallback : CallbackRegister<Args...>::getCallbacks() )
			{
				pairListenerCallback.second( p_params... );
			}
		}
	};
} // namespace VTX::App::Core

#endif
