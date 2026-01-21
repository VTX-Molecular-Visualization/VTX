#ifndef __VTX_UTIL_THREAD__
#define __VTX_UTIL_THREAD__
#include <atomic>
#include <exception>
#include <map>
#include <memory>
#include <mutex>

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
					return obj.stop_requested();
				else
					return false;
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
		std::unique_ptr<_model> _ptr = std::unique_ptr<_wrapper<_void>>();
	};

	/**
	 * @brief Class that expect to be progressively filled by one thread while beeing red by another.
	 * @tparam ElementT Type of element contained in the vector
	 */
	template<typename ElementT>
	class ThreadedVector
	{
	  public:
		ThreadedVector() = delete;
		ThreadedVector( const size_t & p_final_size ) : max_size( p_final_size )
		{
			_collection.reserve(
				p_final_size
			); // This guarantees element references are valid regardless of the moment it has been accessed
		}

		inline size_t size() const noexcept { return _collection.size(); }

		inline bool isAvailable( const size_t & p_index ) const
		{
			if ( _lastIndexAvailable == SIZE_MAX )
				return false;
			if ( p_index <= _lastIndexAvailable )
				return true;
			return false;
		}

		void get( const size_t & p_index, ElementT *& p_elementPtr ) noexcept
		{
			if ( p_index <= _lastIndexAvailable )
				p_elementPtr = &_collection[ p_index ];
			else
				p_elementPtr = nullptr;
		}

		void get( const size_t & p_index, ElementT *& p_elementPtr ) const noexcept
		{
			if ( p_index <= _lastIndexAvailable )
				p_elementPtr = &_collection[ p_index ];
			else
				p_elementPtr = nullptr;
		}

		void addNewElement( ElementT p_newElement )
		{
			if ( max_size <= _collection.size() ) // Enforces The max size to make sure the underlying vector doesn't
												  // relocate the array of elements someplace else
				throw std::runtime_error( "ThreadedVector maximum size reached." );
			_collection.push_back( std::move( p_newElement ) );
			_lastIndexAvailable = _collection.size() - 1;
		}

		const size_t max_size = 0;

	  private:
		std::vector<ElementT>		_collection;
		mutable std::atomic<size_t> _lastIndexAvailable = SIZE_MAX;
	};

} // namespace VTX::Util
#endif
