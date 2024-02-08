#ifndef __VTX_UTIL_CALLBACK__
#define __VTX_UTIL_CALLBACK__

#include <cassert>
#include <functional>

namespace VTX::Util
{
	template<typename... Args>
	class Callback
	{
	  public:
		using Func = std::function<void( const Args &... )>;

		inline void add( const Func & p_callback ) { _callbacks.push_back( p_callback ); }
		inline void remove( const Func & p_callback ) { std::erase( _callbacks, p_callback ); }
		inline void clear() { _callbacks.clear(); }

		inline void operator()( const Args &... p_args ) const
		{
			for ( const Func & callback : _callbacks )
			{
				assert( callback );

				callback( p_args... );
			}
		}

		inline Callback & operator+=( const Func & p_func )
		{
			add( p_func );
			return *this;
		}

		inline Callback & operator-=( const Func & p_func )
		{
			remove( p_func );
			return *this;
		}

	  private:
		std::vector<Func> _callbacks;
	};

} // namespace VTX::Util

#endif
