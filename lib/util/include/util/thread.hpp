#ifndef __VTX_UTIL_THREAD__
#define __VTX_UTIL_THREAD__
#include <memory>

namespace VTX::Util
{
	/**
	 * @brief Class responsible for querying wether a execution should stop or not.
	 */
	class StopToken
	{
		struct _model
		{
			virtual ~_model()			  = default;
			virtual bool stop_requested() = 0;
		};

		template<typename T>
		struct _wrapper final : public _model
		{
			T obj;
			_wrapper( T && p_ ) : obj( std::forward<T>( p_ ) ) {}

			virtual bool stop_requested() override { return obj.stop_requested(); }
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

		bool stop_requested() { return ( _ptr != nullptr ) && _ptr->stop_requested(); }

	  private:
		std::unique_ptr<_model> _ptr = nullptr;
	};
} // namespace VTX::Util
#endif
