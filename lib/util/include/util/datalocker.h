#include <mutex>
#include <optional>
#include <util/exceptions.hpp>

namespace VTX::Util
{
	template<class ReservedDataType>
	class ReservedData;
	template<class DataType>
	class DataLocker;

	// Utility class that aim to provide a simple and concise way to read and write data from multiple threads
	template<class DataType>
	class DataLocker
	{
		DataType _data;
		mutable std::mutex
			_mutex; // This member is mutable so the lock can be tested on a const DataLocker, which make sense as we
					// expect a const object to prevent Data modification but to be lockable anyway

	  public:
		// Default-construct inner data class/struct provided in class template
		DataLocker()  = default;
		~DataLocker() = default;

		DataLocker( DataType && d ) : _data( std::move( d ) ) {}

		// Open the datalocker to be granted an exclusive scoped usage of the datastruct inside
		ReservedData<DataType>		 open() noexcept { return ReservedData( _data, _mutex ); }
		ReservedData<const DataType> open() const noexcept { return ReservedData<const DataType>( _data, _mutex ); }

		bool is_data_available() const noexcept
		{
			if ( _mutex.try_lock() )
			{
				_mutex.unlock();
				return true;
			}
			return false;
		}
	};

	// reference to the inner datastruct guaranteed to be concurrency-safe
	template<class ReservedDataType> // ReservedDataType can be const !
	class ReservedData
	{
		ReservedDataType *						   _dataPtr = nullptr;
		std::optional<std::lock_guard<std::mutex>> _lock; // default constructed objects have no locks
		bool									   _defaultConstructed = true;

		ReservedData( ReservedDataType & d, std::mutex & m ) : _dataPtr( &d ), _lock( m ), _defaultConstructed( false )
		{
		}
		ReservedData() = default;

		// Used when we try to refence the non-const version of the type
		template<class T>
		struct RemoveConst
		{
			typedef T type;
		};

		template<class T>
		struct RemoveConst<const T>
		{
			typedef T type;
		};

		friend DataLocker<ReservedDataType>;
		friend DataLocker<typename RemoveConst<ReservedDataType>::type>; // Very nice trick to befriend a datalocker
																		 // while being a const data

	  public:
		class AttemptedUseOnDefaultConstructed : public Exception
		{
		};

		ReservedDataType * operator->()
		{
			if ( _defaultConstructed )
				throw AttemptedUseOnDefaultConstructed();
			return _dataPtr;
		}
		const ReservedDataType * operator->() const
		{
			if ( _defaultConstructed )
				throw AttemptedUseOnDefaultConstructed();
			return _dataPtr;
		}
		ReservedDataType & operator*()
		{
			if ( _defaultConstructed )
				throw AttemptedUseOnDefaultConstructed();
			return *_dataPtr;
		}
		const ReservedDataType & operator*() const
		{
			if ( _defaultConstructed )
				throw AttemptedUseOnDefaultConstructed();
			return *_dataPtr;
		}
	};
} // namespace VTX::Util
