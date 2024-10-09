#ifndef __VTX_UTIL_CALLBACK__
#define __VTX_UTIL_CALLBACK__

#include <algorithm>
#include <cassert>
#include <functional>
#include <map>
#include <utility>

namespace VTX::Util
{
	using CallbackId = size_t;

	template<typename... Args>
	class Callback
	{
	  public:
		using Func = std::function<void( Args... )>;

		Callback() = default;

		template<typename Callable>
		CallbackId add( Callable && p_callback )
		{
			_callbacks.emplace( _nextId++, std::forward<Callable>( p_callback ) );
			return _nextId - 1;
		}

		void remove( const CallbackId p_id )
		{
			assert( _callbacks.contains( p_id ) );
			_callbacks.erase( p_id );
		}

		inline void clear() { _callbacks.clear(); }

		inline void operator()( Args... p_args ) const
		{
			for ( const auto & callback : _callbacks )
			{
				callback.second( p_args... );
			}
		}

		template<typename Callable>
		inline CallbackId operator+=( Callable && p_func )
		{
			return add( std::forward<Callable>( p_func ) );
		}

		inline void operator-=( const CallbackId p_id ) { remove( p_id ); }

	  private:
		std::map<CallbackId, Func> _callbacks;
		CallbackId				   _nextId = 0;
	};
} // namespace VTX::Util

#endif
