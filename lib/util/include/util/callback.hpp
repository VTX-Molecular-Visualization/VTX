#ifndef __VTX_UTIL_CALLBACK__
#define __VTX_UTIL_CALLBACK__

#include <cassert>
#include <functional>
#include <memory>
#include <vector>

namespace VTX::Util
{
	template<typename... Args>
	class Callback
	{
	  public:
		using Func = std::function<void( const Args &... )>;

		Callback() = default;

		inline void add( const Func & p_callback ) { _callbacks.push_back( p_callback ); }

		/*
		inline void remove( const Func * const p_func )
		{
			std::erase_if(
				_callbacks, [ &p_func ]( const std::unique_ptr<Func> & p_e ) { return p_e.get() == p_func; }
			);
		}
		*/

		inline void clear() { _callbacks.clear(); }

		inline void operator()( const Args &... p_args ) const
		{
			for ( const auto & callback : _callbacks )
			{
				callback( p_args... );
			}
		}

		inline void operator+=( const Func & p_func ) { add( p_func ); }

		/*
		inline Callback & operator-=( const Func * const p_func )
		{
			remove( p_func );
			return *this;
		}
		*/

	  private:
		std::vector<Func> _callbacks;
	};

} // namespace VTX::Util

#endif
