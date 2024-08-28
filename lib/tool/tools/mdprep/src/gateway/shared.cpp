#include <deque>
#include <mutex>
#include <optional>
//
#include "tools/mdprep/gateway/shared.hpp"

namespace VTX::Tool::Mdprep::Gateway
{
	namespace
	{
		template<typename T>
		struct Callback
		{
			Callback( T & p_ ) : _ptr( &p_ ) {}
			T *	 _ptr;
			void operator()( const JobUpdate & p_ ) noexcept { _ptr->addUpdate( p_ ); }
		};
	} // namespace
	class JobUpdateIntermediate::Impl
	{
		std::optional<JobUpdateCallback> _subscribedCallback;
		std::deque<JobUpdate>			 _updateBacklog;
		mutable std::mutex				 _mutex;

	  public:
		void subscribe( JobUpdateCallback p_ ) noexcept
		{
			_subscribedCallback.emplace( std::move( p_ ) );
			std::lock_guard<std::mutex> lock( _mutex );

			while ( not _updateBacklog.empty() )
			{
				_subscribedCallback.value()( _updateBacklog.front() );
				_updateBacklog.pop_front();
			}
		}

		JobUpdateCallback getUpdateCallback() noexcept { return Callback( *this ); }

		void addUpdate( const JobUpdate & p_ ) noexcept
		{
			std::lock_guard<std::mutex> lock( _mutex );
			if ( _subscribedCallback.has_value() )
			{
				_subscribedCallback.value()( p_ );
				return;
			}
			_updateBacklog.push_back( p_ );
		}
	};
	void JobUpdateIntermediate::Del::operator()( Impl * p_ ) noexcept { delete p_; }

	JobUpdateIntermediate::JobUpdateIntermediate() : _impl( new Impl() ) {}
	void JobUpdateIntermediate::subscribe( JobUpdateCallback p_ ) noexcept { _impl->subscribe( std::move( p_ ) ); }
	JobUpdateCallback JobUpdateIntermediate::getUpdateCallback() noexcept { return _impl->getUpdateCallback(); }
} // namespace VTX::Tool::Mdprep::Gateway
