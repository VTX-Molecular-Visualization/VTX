#ifndef __VTX_UTIL_THREAD_STOP_TOKEN__
#define __VTX_UTIL_THREAD_STOP_TOKEN__

#include <stop_token>

namespace VTX::Util::Thread
{
	using StopToken = std::stop_token;

	/**
	 * @brief Class responsible for querying wether a execution should stop or not.
	 */

	/*
	class StopToken
	{
		struct _model
		{
			virtual ~_model()			  = default;
			virtual bool stop_requested() = 0;
		};

		struct _void
		{
		};

		template<typename T>
		struct _wrapper final : public _model
		{
			T obj;

			_wrapper( T && p_ ) : obj( std::forward<T>( p_ ) ) {}

			virtual bool stop_requested() override
			{
				if constexpr ( not std::same_as<T, _void> )
				{
					return obj.stop_requested();
				}
				else
				{
					return false;
				}
			}
		};

	  public:
		StopToken() = default;

		template<typename T>
		StopToken( T && p_ ) : _ptr( new _wrapper<T>( std::forward<T>( p_ ) ) )
		{
			static_assert(
				requires( T t ) {
					{ t.stop_requested() } -> std::convertible_to<bool>;
				}, "You need to implement the \"bool stop_requested()\" method."
			);
		}

		bool stop_requested() { return _ptr->stop_requested(); }

	  private:
		std::shared_ptr<_model> _ptr { new _wrapper<_void>( _void() ) };
	};
	*/
} // namespace VTX::Util::Thread
#endif
